#include "TelegramBot.hpp"
#include<iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

nlohmann::json BotT::sendRequest(std::string command, const nlohmann::json& content)
{
  try{
    net::io_context ioc;
    net::ssl::context ctx(boost::asio::ssl::context::tlsv13_client);

    net::ssl::stream<boost::beast::tcp_stream> stream(ioc, ctx);
    tcp::resolver resolver(ioc);

    auto const resolverRes = resolver.resolve("api.telegram.org", "443");
    beast::get_lowest_layer(stream).connect(resolverRes);

    stream.handshake(boost::asio::ssl::stream_base::handshake_type::client);

    http::request<http::string_body> request(http::verb::post, "/bot" + _token + "/" + command, 11);
    request.set(http::field::host, "api.telegram.org");
    request.set(http::field::content_type, "application/json");
    
  
      request.body() = content.dump();
      request.prepare_payload();
      http::write(stream, request);
  
    beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(stream,buffer, response);

    boost::system::error_code ec;
    stream.shutdown(ec);

    if(ec != boost::asio::ssl::error::stream_truncated)
    {
      throw beast::system_error(ec);
    }

    return nlohmann::json::parse(response.body());
  }
  catch(const std::exception &e)
  {
    throw std::runtime_error(std::string("Request failed: ") + e.what());
  }

}


void BotT::sendMessage(std::string messageText, std::string chat_id)
{
  nlohmann::json response;
  response["chat_id"] = chat_id;
  response["text"] = messageText;
  sendRequest("sendMessage", response);
}



void BotT::processUpdates()
{
  nlohmann::json request;
  if(!_lastUpdateId.empty())
    request["offset"] = std::stoi(_lastUpdateId) + 1;

  nlohmann::json updates = sendRequest("getUpdates", request);

  if(!updates.contains("result"))
    return;

  for(nlohmann::json& update : updates["result"])
  {
    if(update.contains("update_id"))
      _lastUpdateId  = std::to_string(update["update_id"].get<int>());
    if(update.contains("message") && update["message"].contains("text"))
    {
      std::string chat_id = std::to_string(update["message"]["chat"]["id"].get<uint64_t>());
      std::string text = update["message"]["text"].get<std::string>();

      std::cout << "Receive message: \n";
      std::cout << text << " [by "<< update["message"]["from"]["username"].get<std::string>() <<  "]"<< std::endl;
      std::cout << "chat_id: " << chat_id << std::endl;

      std::string response = "Unknown command ;(";
      if(_functionTable.count(text))
        response = _functionTable[text](update);

      sendMessage(response, chat_id);
    }
  }
}

void BotT::startBot()
{
  
  setMyCommands(_commandList);
//  std::cout << _commandList << std::endl << std::endl;
//  std::cout << getCommandList() << std::endl;
  
  
  std::cout << "The bot has started" << std::endl;
  while(1)
  {
    try{
      
      processUpdates();

    }catch(const std::exception &e)
    {
      std::cerr <<"Error: "<< e.what() << std::endl;
    }
  }
}


void BotT::setMyCommands(nlohmann::json& commands)
{
  sendRequest("setMyCommands", commands);
}

nlohmann::json BotT::getCommandList()
{

  return sendRequest("getMyCommands");
}


void BotT::addFunction(BotCommand command, std::function<std::string(const nlohmann::json&)> foo)
{
  _functionTable["/" + command.first] = foo;
  _commandList["commands"].push_back({{"command", command.first},{"description", command.second}});

}

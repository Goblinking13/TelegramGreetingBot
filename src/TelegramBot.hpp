#ifndef TelegramBot_hpp
#define TelegramBot_hpp

#include<string>
#include<nlohmann/json.hpp>
#include<functional>
#include<unordered_map>
#include<iostream>

// first - command, second - description
typedef std::pair<std::string,std::string> BotCommand;

class BotT
{
  std::string _token;
  std::string _lastUpdateId;
  std::unordered_map<std::string, std::function<std::string(const nlohmann::json& message)> > _functionTable;
  nlohmann::json _commandList;
  
  public:
  
  BotT(std::string token) : _token(token) { _commandList.clear();};
  
  nlohmann::json sendRequest(std::string command, const nlohmann::json& content  = NULL);
  void sendMessage(std::string message, std::string chat_id);
  void processUpdates();
  void startBot();
  // add function "foo" to bot command list
  void addFunction(BotCommand command, std::function<std::string(const nlohmann::json&)> foo);
  void setMyCommands(nlohmann::json& coommands);
  nlohmann::json getCommandList();
};

#endif 

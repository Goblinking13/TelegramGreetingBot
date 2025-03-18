#include <iostream>
#include<nlohmann/json.hpp>
#include"TelegramBot.hpp"

std::string helloFoo(const nlohmann::json& msg)
{
    return "Hello " + msg["message"]["from"]["first_name"].get<std::string>();
}



int main(int argc, const char * argv[]) {

  std::string TOKEN = "PUT YOUR TELEGRAM BOT TOKEN HERE";

  BotT bot(TOKEN);
  bot.addFunction({"hello", "say hello"}, helloFoo);
  bot.startBot();
  
  
  
  
  return 0;
}



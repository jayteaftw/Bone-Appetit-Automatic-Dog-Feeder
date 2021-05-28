#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_MODULE
#include <Dabble.h>
#include <Servo.h>

Servo myservo;
String message;

//Holds info on how the automatic dog feeder should act.
struct Settings{
  int cups = 1; //Cup size of food: 1 or 2 cups
  int autofeed = 1; //Signal if autofeeding functionality is on: 1 is on and 0 is off.
  unsigned long timer = 43200; //The clock of the device. Deault 12:00 
  unsigned long offset_time = 0; //Offset time used to implement clock with millis().
  unsigned long schedule[3] = {28800, 57600, 86410}; //Schedule when device will feed: has 3 configurable timeslots. Default 8:AM and 6:PM Feeding Times
  int foodLog[3] = {0,0,0}; //Used to stop over feeding.
}settings;

struct Settings mySettings;

//Changes Entered Hours and Minutes into seconds and returns the seconds.
unsigned long HoursAndMinutesToSeconds()
{
  unsigned long hour, minutes;
  Terminal.print("Enter Hour: ");
  while(1){
    Dabble.processInput();
    if(Terminal.available()){
      int x = Terminal.readNumber();   
      hour = (unsigned long)x%24;
      break;  
    }
    
   }
   Terminal.print("Enter Minutes: ");
   while(1){
     Dabble.processInput();
     if(Terminal.available()){
       int x = Terminal.readNumber();   
       minutes = (unsigned long)x%60;  
       break; 
     }
   }
   return hour*3600 + minutes*60;
        
}



//Allows user within the app to change settings
void changeSettings(){
  int flag = 0;
 
  Terminal.println("What can I help you with?");
  Terminal.println(String("\n Commands\nA: Amount\nF: AutoFeed, \nT: Change Time \nS: Schedule \nH: Help \nE: Exit "));
  while(flag == 0)
  {
    Dabble.processInput();
    //Amount of Food
    if(Terminal.compareString("A"))
    {
       if( mySettings.cups == 1)
        Terminal.print("\nBowl is set to 1 Cup. Would you like to change it to 2? (Y, N)");
       
       else
        Terminal.print("\nBowl is set to 2 Cups. Would you like to change it to 1? (Y, N)");

       //Loops until user picks Y or N. If Y, food will change from 1 to 2 cups and vice versa.
       String message;
       while(1){
        Dabble.processInput();
        if(Terminal.available())
        {
          if(Terminal.compareString("Y"))
          {
            mySettings.cups = (mySettings.cups %2) + 1;
            message = "Settings Changed!";
            break;
          }
          if(Terminal.compareString("N"))
          {
            message = "Settings Not Changed.";
            break;
          }
        }
        delay(50);
      }
      Terminal.print(message);
     
   
  
    }
    //Changes Autofeed Setting
    else if(Terminal.compareString("F"))
    {
    
      if( mySettings.autofeed == 1)
        Terminal.print("\nAutofeed is Currently On. Would you like to turn it off? (Y, N)");
  
      else
        Terminal.print("\nAutofeed is Currently Off. Would you like to turn it on? (Y, N)");
      
      //Loops until user picks Y or N. If Y, autfood will change from on to off cups and vice versa.  
      while(1){
        Dabble.processInput();
        if(Terminal.available())
        {
          if(Terminal.compareString("Y"))
          {
            mySettings.autofeed = (mySettings.autofeed + 1)%2;
            message = "Settings Changed!";
            break;
          }
          if(Terminal.compareString("N"))
          {
            message = "Settings Not Changed.";
            break;
          }
        }
        delay(50);
      }
      Terminal.print(message);
    }

    //Change Time
    else if(Terminal.compareString("T")){

        //Calls HoursandMinutesToSeconds function to get an Hour and Minutes time. Time is then set as the clock of the device.
        //The current millis() is then saved as the offset.
        mySettings.timer = HoursAndMinutesToSeconds();
        mySettings.offset_time = millis()/1000;
        Terminal.print("Time has been changed!");
        
    }
    //Scheduler
    else if(Terminal.compareString("S")){
      Terminal.print("\nWould you like to add or delete a time?\n(Add, Del, N)");
      int flag = -1;
      String message = "Settings Changed!";
      while(1){
        Dabble.processInput();
        if(Terminal.available())
        {
          if(Terminal.compareString("Add"))
          {
            flag = 1;
            break;
          }
          if(Terminal.compareString("Del"))
          {
            flag = 0;
            break;
          }
          if(Terminal.compareString("N"))
          {
            message = "Settings Not Changed.";
            break;
          }
        }
        delay(50);
      }
      
      if(flag != -1){
         int x;
         Terminal.print("Which time slot? (1,2,3)");
         while(1){
          Dabble.processInput();
          if(Terminal.available()){
            x = Terminal.readNumber();
            if( x == 1 || x == 2 || x == 3)    
              break; 
          }
         }
         //Delete slot
         if(flag == 0)
          mySettings.schedule[x-1] = 86410; //Time is deleted
         
         //Add or Replace slot
         else if(flag == 1)
         {
           mySettings.schedule[x-1] = HoursAndMinutesToSeconds();
         }
       }
      
      Terminal.print(message);
      delay(100);
      
    }
    //HELP
    else if(Terminal.compareString("H")){
      //Prints out the command list.
      Terminal.println(String("\n Commands\nA: Amount\nF: AutoFeed, \nT: Change Time \nS: Schedule \nH: Help \nE: Exit "));
    }
    //EXIT
    else if(Terminal.compareString("E")){
      //Exits the Settings menu.
      flag = 1;
    }
    Serial.println(Terminal.read());
    delay(100);
  }
  Terminal.print("Exiting Settings");
}


unsigned long currentTime()
{
  //Returns the current time in seconds.
  return (mySettings.timer + millis()/1000 - mySettings.offset_time)%86400;
}



void printTime()
{
  //Prints the Current Time of the device, and all the scheduled feeding times.
  unsigned long sec = currentTime();
  unsigned long h = sec/3600;
  unsigned long m = (sec -(3600*h))/60;
  String message = h + String(":") + m;
  Terminal.print("The time is");
  Terminal.print(message);

  int flag = 0;
  Terminal.print("Times Scheduled");
  //Loops through all 3 scheduled slots. Checks to see if there is first a time scheduled 
  //and if so prints the slot in a readable format.
  for(int i = 0; i < 3; i++)
  {
     if(mySettings.schedule[i] != 86410 )
     {
      flag = 1;
      sec = mySettings.schedule[i];
      h = sec/3600;
      m = (sec -(3600*h))/60;
      message =  String("Slot:")+ (i+1) + String("   ") + h + String(":") + m;
      Terminal.print(message);
     }
  }
  if(flag == 0)
    Terminal.print("No Scheduled Times");
 
}

//Feeding Algorithm
void timeToEat(int man)
{
  //Enables the Feeding function. The amount of food and autofeed is determined by mySettings.
  if(mySettings.autofeed == 0 && man == 0)
  {
    Terminal.print("It is time to eat!");
    Terminal.print("Should I feed? (Y or N)");
    while(1){
        Dabble.processInput();
        if(Terminal.available())
        {
          if(Terminal.compareString("Y"))
          {
            break;
          }
          if(Terminal.compareString("N"))
          {
            Terminal.print("Feeding Stopped.");
            delay(61000);
            return;
          }
        }
        delay(50);
     }
  }
  int feedTime;
  if(mySettings.cups == 2)
  {
    Terminal.print("Now Feeding 2 Cups.");
    feedTime = 3000;
  }
  else{ 
    Terminal.print("Now Feeding 1 Cup.");
    feedTime = 2000;
  }

  myservo.write(180);
  delay(feedTime);
  myservo.write(0);

  Terminal.print("Feeding Complete!");
 
  
}

//Checks to see if it is Time to eat.
void isItTimeToEat()
{
  unsigned long sec,h,m,hSch, mSch;
  sec = currentTime();
  h = sec/3600;
  m = (sec -(3600*h))/60;
  //Loops through all scheduled times. 
  for(int i = 0; i < 3; i++)
  {
     if(mySettings.schedule[i] != 86410 )
     {
       sec = mySettings.schedule[i];
       hSch = sec/3600;
       mSch = (sec -(3600*h))/60;
       
       //If a time is scheduled the device then checks if it is time to feed by comparing if the current hours and minutes equal itself. 
       //It also checks if the foodLog is zero to stop over feeding
       if(h == hSch && m == mSch && mySettings.foodLog[i] == 0)
       {
         timeToEat(0);
         mySettings.foodLog[i] = 1;
       }
       //The device then checks if the scheduled time does not equal the current time and if the foodLog is equal to 1 so it can reset it by setting the foodLog equal to 0.
       else if(h != hSch && m != mSch && mySettings.foodLog[i] == 1)
        mySettings.foodLog[i] = 0;
       
     }
  }
}


void setup() {
  Serial.begin(250000);
  Dabble.begin(9600);    
  myservo.attach(9);  
  myservo.write(0);
  delay(500);
  Serial.print("Device Ready to be Connected!");
  Dabble.processInput();
  Terminal.print("High Welcome to Bone Appetit!");
  Terminal.print("Here are some commands.");
  Terminal.print(" \nF: Feed \nS: Settings \nT: Time \nH: Help");

}

void loop() {
  Dabble.processInput();

  //Checks if the user has inputed a command.
  if(Terminal.available())
  {
    
    Serial.println(Terminal.readString());
    //Manually tells the device to feed.
    if(Terminal.compareString("F")){
      timeToEat(1);
    }
    //Opens Setting Menu.
    else if(Terminal.compareString("S")){
      changeSettings();
    }
    //Repeats the Command list.
    else if(Terminal.compareString("H")){
      Terminal.print(" \nF: Feed \nS: Settings \nT: Time \nH: Help");
    }
    //Prints out the current time and scheduled feeding times.
    else if(Terminal.compareString("T")){
      printTime();
    }
    else{
      //Error
    }
  }
  //Compares Scheduled times and current time to see if it is time to feeed.
  isItTimeToEat();
  myservo.write(0);
  delay(100);
}

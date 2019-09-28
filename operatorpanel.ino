#include <limits.h>

static const int PinHddIn = 2;
static const int PinHddOut = 3;
static const int PinDisplayReset = 4;
static const int PinDisplayStrobe = 5;
static const int DisplaySize = 8;

namespace Display
{
  namespace
  {
    void Reset()
    {
      digitalWrite(PinDisplayStrobe, 1);
      digitalWrite(PinDisplayReset, 1);
      digitalWrite(PinDisplayReset, 0);
    }

    void SendChar(const char c, const int iDigit)
    {
      const int iCount = c<<3 | ((DisplaySize-1) - (iDigit&7));
      for (int i = 0; i < iCount; i++)  //Strobe out character
      {
        digitalWrite(PinDisplayStrobe, 0);
        digitalWrite(PinDisplayStrobe, 1);
      }
    
      //Confirm character and prepare for next
      digitalWrite(PinDisplayReset, 1);
      digitalWrite(PinDisplayStrobe, 0);
      digitalWrite(PinDisplayStrobe, 1);
      digitalWrite(PinDisplayReset, 0);
    }

    void SendString(const char* const pszString)
    {
      const char* pC = pszString;
      for(int i=0; i<8; i++)
      {
        const char c = *pC ? *pC++ : ' ';
        SendChar(c, i);
      }
    }
  }
}


static void HddLed()
{
  digitalWrite(PinHddOut, !digitalRead(PinHddIn)); //Turn negative-switched signal into positive-switched
}

void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PinDisplayReset, OUTPUT);
  pinMode(PinDisplayStrobe, OUTPUT);
  
  pinMode(PinHddIn, INPUT_PULLUP);
  pinMode(PinHddOut, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PinHddIn), HddLed, CHANGE);
  
  Display::Reset();
  HddLed();
  Display::SendString("started");
  delay(1000);
}

void loop() 
{
  static const unsigned int TimeOutTime = 5000;
  static unsigned long LastRecvTime = UINT_MAX - TimeOutTime; //Go into timeout immediately
  static int iDigit = 0;
  
  bool bReceived = false;
 
  while (Serial.available())
  {
    const char c = Serial.read();
    Display::SendChar(c, iDigit);
    iDigit = (iDigit+1) % DisplaySize;
    
    bReceived = true;
  }

  const unsigned long CurrTime = millis();
  if (bReceived)
  {
    LastRecvTime = CurrTime;
  }
  const unsigned long TimeSinceRecv = CurrTime - LastRecvTime;
  const bool bDemoMode = TimeSinceRecv > TimeOutTime;
  
  if (bDemoMode)
  {
    iDigit = 0; //Next time we receive data start from first digit
    char szDemo[DisplaySize+1];
    const int iSeg = ((TimeSinceRecv-TimeOutTime) / 500) % DisplaySize;
    for (int i = 0; i < DisplaySize; i++)
    {
      Display::SendChar(i == iSeg ? '.' : ' ', i);
    }
    
  }
}

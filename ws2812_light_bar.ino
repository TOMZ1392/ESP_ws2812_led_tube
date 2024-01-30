
/*
led array driver
 */

#include <FastLED.h>


#define NUM_LEDS 68  // number of leds
#define DATA_PIN 1    // Where the LED DATA wire is GPIO0 (0) for ESP01 ??

#define RAMP_MAX_BRIGHTNESS  40
#define RAMP_STEP_SIZE        1
#define RAMP_STEP_INTERVAL    50 



CRGB leds[NUM_LEDS];


uint32_t ledSectionColorMap[] = {
  CRGB::AliceBlue,
  CRGB::Amethyst,
  CRGB::AntiqueWhite,
  CRGB::Aqua,
  CRGB::Aquamarine,
  CRGB::Azure,
  CRGB::Beige,
  CRGB::Bisque,
  CRGB::BlanchedAlmond,
  CRGB::Blue,
  CRGB::BlueViolet,
  CRGB::Brown,
  CRGB::BurlyWood,
  CRGB::CadetBlue,
  CRGB::Chartreuse,
  CRGB::Chocolate,
  CRGB::Coral,
  CRGB::CornflowerBlue,
  CRGB::Cornsilk,
  CRGB::Crimson,
  CRGB::Cyan,
  CRGB::DarkBlue,
  CRGB::DarkCyan,
  CRGB::DarkGoldenrod,
  CRGB::DarkGray,
  CRGB::DarkGrey,
  CRGB::DarkGreen,
  CRGB::DarkKhaki,
  CRGB::DarkMagenta,
  CRGB::DarkOliveGreen,
  CRGB::DarkOrange,
  CRGB::DarkOrchid,
  CRGB::DarkRed,
  CRGB::DarkSalmon,
  CRGB::DarkSeaGreen,
  CRGB::DarkSlateBlue,
  CRGB::DarkSlateGray,
  CRGB::DarkSlateGrey,
  CRGB::DarkTurquoise,
  CRGB::DarkViolet,
  CRGB::DeepPink,
  CRGB::DeepSkyBlue,
  CRGB::DimGray,
  CRGB::DimGrey,
  CRGB::DodgerBlue,
  CRGB::FireBrick,
  CRGB::FloralWhite,
  CRGB::ForestGreen,
  CRGB::Fuchsia,
  CRGB::Gainsboro,
  CRGB::GhostWhite,
  CRGB::Gold,
  CRGB::Goldenrod,
  CRGB::Gray,
  CRGB::Grey,
  CRGB::Green,
  CRGB::GreenYellow,
  CRGB::Honeydew,
  CRGB::HotPink,
  CRGB::IndianRed,
  CRGB::Indigo,
  CRGB::Ivory,
  CRGB::Khaki,
  CRGB::Lavender,
  CRGB::LavenderBlush,
  CRGB::LawnGreen,
  CRGB::LemonChiffon,
  CRGB::LightBlue,
  CRGB::LightCoral,
  CRGB::LightCyan,
  CRGB::LightGoldenrodYellow,
  CRGB::LightGreen,
  CRGB::LightGrey,
  CRGB::LightPink,
  CRGB::LightSalmon,
  CRGB::LightSeaGreen,
  CRGB::LightSkyBlue,
  CRGB::LightSlateGray,
  CRGB::LightSlateGrey,
  CRGB::LightSteelBlue,
  CRGB::LightYellow,
  CRGB::Lime,
  CRGB::LimeGreen,
  CRGB::Linen,
  CRGB::Magenta,
  CRGB::Maroon,
  CRGB::MediumAquamarine,
  CRGB::MediumBlue,
  CRGB::MediumOrchid,
  CRGB::MediumPurple,
  CRGB::MediumSeaGreen,
  CRGB::MediumSlateBlue,
  CRGB::MediumSpringGreen,
  CRGB::MediumTurquoise,
  CRGB::MediumVioletRed,
  CRGB::MidnightBlue,
  CRGB::MintCream,
  CRGB::MistyRose,
  CRGB::Moccasin,
  CRGB::NavajoWhite,
  CRGB::Navy,
  CRGB::OldLace,
  CRGB::Olive,
  CRGB::OliveDrab,
  CRGB::Orange,
  CRGB::OrangeRed,
  CRGB::Orchid,
  CRGB::PaleGoldenrod,
  CRGB::PaleGreen,
  CRGB::PaleTurquoise,
  CRGB::PaleVioletRed,
  CRGB::PapayaWhip,
  CRGB::PeachPuff,
  CRGB::Peru,
  CRGB::Pink,
  CRGB::Plaid,
  CRGB::Plum,
  CRGB::PowderBlue,
  CRGB::Purple,
  CRGB::Red,
  CRGB::RosyBrown,
  CRGB::RoyalBlue,
  CRGB::SaddleBrown,
  CRGB::Salmon,
  CRGB::SandyBrown,
  CRGB::SeaGreen,
  CRGB::Seashell,
  CRGB::Sienna,
  CRGB::Silver,
  CRGB::SkyBlue,
  CRGB::SlateBlue,
  CRGB::SlateGray,
  CRGB::SlateGrey,
  CRGB::Snow,
  CRGB::SpringGreen,
  CRGB::SteelBlue,
  CRGB::Tan,
  CRGB::Teal,
  CRGB::Thistle,
  CRGB::Tomato,
  CRGB::Turquoise,
  CRGB::Violet,
  CRGB::Wheat,
  CRGB::White,
  CRGB::WhiteSmoke,
  CRGB::Yellow,
  CRGB::YellowGreen,
  CRGB::FairyLight,
  CRGB::FairyLightNCC
};

void ledArrayInit();
void setLedArrayProperties(uint16_t,uint16_t,uint8_t);
void runLedArraySimTest();
void colorBreathMode();

void ledArrayInit() 
{
  pinMode(DATA_PIN, FUNCTION_3);
  //pinMode(3, FUNCTION_3);  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(0);
  FastLED.show();
}


void setLedArrayProperties(uint16_t colorIdx, uint16_t ledBrightness)
 {
  uint16_t tmpIdx = 0;
  FastLED.setBrightness(ledBrightness);
  if (colorIdx >= (sizeof(ledSectionColorMap) / sizeof(ledSectionColorMap[0]))) 
  {
    return;
  }

  for (; tmpIdx < NUM_LEDS; tmpIdx++) 
  {
    leds[tmpIdx] = ledSectionColorMap[colorIdx];
    
  }
  FastLED.show();
  
}

void runLedArraySimTest() 
{
  uint8_t bgt = 19, color = 0;
  for (; bgt < 20; bgt) 
  {
    for (color = 0; color < 46; color++) 
    {
      setLedArrayProperties(color, bgt, 0);
      delay(300);
    }
  }
}

void colorBreathMode()
{
  static uint8_t col=0;
  static uint8_t brt=0;
  static long startime=millis();
  static bool isRising=true;
  if(millis() - startime > RAMP_STEP_INTERVAL)
  {
      setLedArrayProperties(col,brt);
      if(isRising)
      {
        if(brt < RAMP_MAX_BRIGHTNESS)
        {
          brt+=RAMP_STEP_SIZE;
          
        }
        else if(brt>=RAMP_MAX_BRIGHTNESS)
        {
          isRising=false;
        }
        else
        {
          brt=RAMP_MAX_BRIGHTNESS;
        }
      }
      else
      {
        if(brt > RAMP_STEP_SIZE)
        {
          brt-=RAMP_STEP_SIZE; 
        }
        else if(brt<=RAMP_STEP_SIZE)
        {
          if(col < (sizeof(ledSectionColorMap) / sizeof(ledSectionColorMap[0])))
          {
            col++;
          }
          else
          {
            col=0;
          }
          
          isRising=true;
          brt=0;
        }
        else
        {
          brt=0;
        }
      }
      startime=millis();
  }

}



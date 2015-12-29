/*
 * Randomly color fade APA102C LEDs without turning off or turning white.  
 * Fades from one of the three primary colors (red, blue, green) to one of
 * the three secondary colors (cyan, magenta, yellow). Each pixel is intilized 
 * to a random primary color. It is then faded into a secondary color by 
 * randomly picking one of the two empty color channels and increasing it.
 * 
 * It is then faded back to a primary color by picking one of the non-empty
 * channels and decreasing it.
 */

#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 10
#define DATAPIN    8
#define CLOCKPIN   9

#define BRIGHTNESS 255
#define COLOR_STEP_INCREMENT 4
#define COLOR_STEP_DELAY 50

/*
  ColorData off =     { 0   , 0   , 0   };
  ColorData white =   { 255 , 255 , 255 };

  ColorData yellow =  { 0   , 255 , 255 };
  ColorData cyan =    { 255 , 0   , 255 };
  ColorData magenta = { 255 , 255 , 0   };

  ColorData blue =    { 255 , 0   , 0   };
  ColorData red =     { 0   , 255 , 0   };
  ColorData green =   { 0   , 0   , 255 };  
*/

struct RGB {
  byte blue;
  byte red;
  byte green;
};

// Use this to cast to uint32_t for Adafruit_DotStar. 
union ColorData {
 byte data[4];
 uint32_t int32;
};

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

// Keep track of the color of each pixel.
ColorData pixels[NUMPIXELS];

// Create a random primary color, used to initilize each pixel with a different starting point.
ColorData getRandomPrimary() {
  int randomIndex = random(0,3);
  Serial.print("rand ");
  Serial.println(randomIndex);
  ColorData randomPrimary = {0   , 0   , 0};
  randomPrimary.data[randomIndex] = 255;
  return randomPrimary;
}

void setup() {
  randomSeed(6);
  Serial.begin(9600);
  delay(500);
  strip.begin();
  
  // Start each pixel at a random primary color.
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels[i] = getRandomPrimary();
    strip.setPixelColor(i, pixels[i].int32);
  }
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

// TODO: Remove me!
int randomIndex;

void loop() {
  // b, r ,g

  // Keep track of which color channel to fade for each pixel.
  int pixel_index[NUMPIXELS];

  // Pick a random color channel to increase.
  Serial.println("Determine increase channel.");

  // Print debug headers.
  Serial.print("Pixel:");
  Serial.print("\t");
  Serial.print("Channels:");
  Serial.println("\t");

  // Loop through each pixel and fade a random channel.
  for (int j = 0; j < NUMPIXELS; j++) {
    Serial.print(j);
    Serial.print(": ");
    Serial.print("\t");
    // Determine the channel to adjust by randomly picking one which is fully off.
    int randomChannel;
    do {
      randomChannel = random(0,3); // 0 - 2.
      Serial.print(randomChannel); 
      Serial.print(" "); 
      // If the channel is already on, try another channel.
    } while (pixels[j].data[randomChannel] == 255);
    // Record the channel for future use.
    pixel_index[j] = randomChannel;
    Serial.println("");
  }
  Serial.println("Start increment.");

  // Transition into secondary color by fading the selected channel for each pixel.
  for (int i = COLOR_STEP_INCREMENT - 1; i <= 256; i = i + COLOR_STEP_INCREMENT) {
    Serial.print(i);
    for (int h = 0; h < NUMPIXELS; h++) {
        Serial.print(" ");
        Serial.print(h);
        
        int index = pixel_index[h];
        pixels[h].data[index] = i;
        strip.setPixelColor(h, pixels[h].int32);
        
        Serial.print("\t");
        Serial.print(pixels[h].int32, HEX);
        Serial.print("\t");
      }
    strip.show();
    delay(COLOR_STEP_DELAY);
    Serial.println("");
  }

  // Pick a random color channel to decrease.
  Serial.println("Determine decrease indicies.");
  for (int j = 0; j < NUMPIXELS; j++) {
    Serial.print(j);
    Serial.print(" Trying: ");
    // Determine the index to adjust by randomly picking until its not 255.
    do {
      randomIndex = random(0,3);
      Serial.print(randomIndex); 
      Serial.print(" "); 
    } while (pixels[j].data[randomIndex] == 0);
    pixel_index[j] = randomIndex;
    Serial.println("");
  }
  Serial.println("Start decrement.");

  // Transition into primary color.
  for (int i = COLOR_STEP_INCREMENT - 1; i <= 256; i = i + COLOR_STEP_INCREMENT) {
    Serial.print(i);
    for (int h = 0; h < NUMPIXELS; h++) {
        Serial.print(" ");
        Serial.print(h);
        int index = pixel_index[h];
        pixels[h].data[index] = 255 - i;
        strip.setPixelColor(h, pixels[h].int32);
        
        Serial.print("\t");
        Serial.print(pixels[h].int32, HEX);
        Serial.print("\t");
    }
    strip.show();
    delay(COLOR_STEP_DELAY);
    Serial.println("");
  }

  delay(COLOR_STEP_DELAY);

}


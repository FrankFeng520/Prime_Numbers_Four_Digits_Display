// Frank Feng
// Prime Number Display

/* ***************************************************
 *                Port Configuration                 *
 *************************************************** */

const int dataPin  = 12;  // 74HC595 pin 8 DS
const int latchPin = 11;  // 74HC595 pin 9 STCP
const int clockPin = 9;   // 74HC595 pin 10 SHCP
const int digit0   = 7;   // 7-Segment pin D4
const int digit1   = 6;   // 7-Segment pin D3
const int digit2   = 5;   // 7-Segment pin D2
const int digit3   = 4;   // 7-Segment pin D1 
const int buttonIncreasePin = 2; // Increase button
const int buttonDecreasePin = 3; // Decrease button

/* ***************************************************
 *                Global Variables                   *
 *************************************************** */

unsigned char table[]= 
    {   0x5F,  // = 0
        0x44,  // = 1
        0x9D,  // = 2
        0xD5,  // = 3
        0xC6,  // = 4
        0xD3,  // = 5
        0xDB,  // = 6
        0x45,  // = 7
        0xDF,  // = 8
        0xC7,  // = 9
    }; 
unsigned char digitDP = 32;  // 0x20 - adds this to digit to show decimal point
unsigned char controlDigits[] = {digit0, digit1, digit2, digit3}; // Control transistor
unsigned char displayDigits[] = {0, 0, 0, 0}; // Store actual display value number
unsigned int nth = 1; // Current nth prime
unsigned char debound = 0; 
int brightness = 90;   // valid range of 0-100, 100=brightest

/* ***************************************************
 *                   Void Setup                      *
 *************************************************** */

void setup() { 
    pinMode(latchPin,OUTPUT);
    pinMode(clockPin,OUTPUT);
    pinMode(dataPin,OUTPUT);
    pinMode(buttonIncreasePin, INPUT_PULLUP);  
    pinMode(buttonDecreasePin, INPUT_PULLUP);
    for (int x=0; x<4; x++){
        pinMode(controlDigits[x],OUTPUT);
        digitalWrite(controlDigits[x],LOW);  // Turns off the digit  
    }
}

/* ***************************************************
 *                   Functions                       *
 *************************************************** */    

void DisplaySegments(int number){
     for (int i = 0; i < 4; i++) {
      // Get individual digit of number w/ modulo
       displayDigits[i] = table[static_cast<int>(number / pow(10, i)) % 10];
    }
    
    for (int x=0; x<4; x++){
        for (int j=0; j<4; j++){
            digitalWrite(controlDigits[j],LOW);    // turn off digits
        }
        digitalWrite(latchPin,LOW);            // Latch = 0, Enable Data
        shiftOut(dataPin,clockPin,MSBFIRST,displayDigits[x]); // Store Data
        digitalWrite(latchPin,HIGH);           // Latch = 1, Send Data
        digitalWrite(controlDigits[x],HIGH);   // turn on one digit
        delay(1);                              // 1 ms delay
    }
    for (int j=0; j<4; j++){
        digitalWrite(controlDigits[j],LOW);    // turn off digits
    } 
}

/* ***************************************************
 *              Willans' Prime Factory                *
 *************************************************** */ 

unsigned long factorial(int num) {
  // Factorial function using recursion
  if (num == 0 || num == 1) {
    return 1;
  } else {
    return num * factorial(num - 1);
  }
}

unsigned long nthPrimeWillans(int n) {
  unsigned long result = 1; 

  for (int i = 1; i <= pow(2, n); i++) {
    float innerSum = 0.0;

    for (int j = 1; j <= i; j++) {
      innerSum += floor(pow(cos(PI * (factorial(j - 1) + 1) / j), 2));
    }
    result += floor(pow(n / innerSum, 1.0 / n));
  }
  return result;
}


/* ***************************************************
 *              Sieve of Eratosthenes                *
 *************************************************** */ 

void sieveOfEratosthenes(bool prime[], int n) {
  for (int p = 2; p * p <= n; p++) {
    if (prime[p]) {
      for (int i = p * p; i <= n; i += p)
        prime[i] = false;
    }
  }
}

unsigned int nthPrimeEratosthenes(int n) {
  const int MAX_SIZE = 8000;  
  bool prime[MAX_SIZE];
  memset(prime, true, sizeof(prime));

  sieveOfEratosthenes(prime, MAX_SIZE);

  int count = 0;
  unsigned long result = 0;

  for (int p = 2; p < MAX_SIZE; p++) {
    if (prime[p]) {
      count++;
      if (count == n) {
        result = p;
        break;
      }
    }
  }

  return result;
}

/* ***************************************************
 *              Sieve of Atkins                      *
 *************************************************** */ 

void sieveOfAtkins(bool *isPrime) {
  const int MAX_LIMIT = 8000;

  int sqrtLimit = static_cast<int>(sqrt(MAX_LIMIT));
  for (int x = 1; x <= sqrtLimit; x++) {
    for (int y = 1; y <= sqrtLimit; y++) {
      int n = (4 * x * x) + (y * y);
      if (n <= MAX_LIMIT && (n % 12 == 1 || n % 12 == 5))
        isPrime[n] = !isPrime[n];

      n = (3 * x * x) + (y * y);
      if (n <= MAX_LIMIT && n % 12 == 7)
        isPrime[n] = !isPrime[n];

      n = (3 * x * x) - (y * y);
      if (x > y && n <= MAX_LIMIT && n % 12 == 11)
        isPrime[n] = !isPrime[n];
    }
  }

  for (int x = 5; x <= sqrtLimit; x++) {
    if (isPrime[x]) {
      for (int y = x * x; y <= MAX_LIMIT; y += x * x) {
        isPrime[y] = false;
      }
    }
  }

  isPrime[2] = isPrime[3] = true;
}

int nthPrimeAtkins(int nth) {
  const int MAX_LIMIT = 8000;
  bool *isPrime = new bool[MAX_LIMIT + 1];
  memset(isPrime, false, (MAX_LIMIT + 1) * sizeof(bool));

  sieveOfAtkins(isPrime);

  int count = 0;
  for (int i = 2; i <= MAX_LIMIT; i++) {
    if (isPrime[i]) {
      count++;
      if (count == nth) {
        delete[] isPrime;
        return i;
      }
    }
  }
}


/* ***************************************************
 *                   Void Loop                       *
 *************************************************** */

void loop() {
 
    // DisplaySegments(nthPrimeWillans(nth)); // Using Willans' Prime Factory
    // DisplaySegments(nthPrimeAtkins(nth)); // Using Sieve of Atkins
    DisplaySegments(nthPrimeEratosthenes(nth)); // Using Sieve of Eratosthenes

  if (digitalRead(buttonIncreasePin) == LOW && debound == 0)
  {
    nth++;
    debound = 1;
  }

  if (digitalRead(buttonDecreasePin) == LOW && debound == 0 && nth > 1)
  {
    nth--;
    debound = 1;
  }

  if (digitalRead(buttonIncreasePin) == HIGH && digitalRead(buttonDecreasePin) == HIGH)
  {
  debound = 0;
  }
}


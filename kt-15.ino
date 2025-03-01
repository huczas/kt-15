#define BUTTON_PIN PB0
#define ALARM_PIN PB1
#define LED_PIN1 PB2
#define LED_PIN2 PB3
#define LED_PIN3 PB4

volatile int currentLED = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long countdownStartTime = 0;
unsigned long alarmToggleTime = 0;   // the last time the alarm state was toggled
bool counting = false;
bool alarm_active = false;
bool alarm_state = false;            // the current state of the alarm (on/off)
int buttonState;            // the current reading from the input pin
int lastButtonState = HIGH; // the previous reading from the input pin (initially HIGH because of INPUT_PULLUP)

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(ALARM_PIN, OUTPUT);
    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    pinMode(LED_PIN3, OUTPUT);

    // Set the initial state of the ALARM_PIN to LOW at the beginning of the program
    digitalWrite(ALARM_PIN, LOW);
}

void loop() {
    checkButton();

    // Display the number in binary on the LEDs
    display_number(currentLED);

    // Check if the button has not been pressed for 2 seconds
    if ((millis() - lastDebounceTime) > 2000 && !counting && !alarm_active) {
        start_timer();
    }

    // If the countdown is active, update the display
    if (counting) {
        if ((millis() - countdownStartTime) >= 60000) { // 1 minute
            countdownStartTime = millis();
            currentLED--;
            if (currentLED <= 0) {
                currentLED = 0;
                counting = false;
                alarm_active = true;
                alarmToggleTime = millis(); // Initialize alarm toggle time
            }
        }
        display_number(currentLED);
    }

    // If the alarm is active, start beeping
    if (alarm_active) {
        if ((millis() - alarmToggleTime) >= 500) { // Toggle alarm state every 500ms
            alarm_state = !alarm_state;
            digitalWrite(ALARM_PIN, alarm_state ? HIGH : LOW);
            alarmToggleTime = millis();
        }
        checkButton(); // Check button during alarm
    }
}

void checkButton() {
    // Read the state of the button
    int reading = digitalRead(BUTTON_PIN);

    // If the button state has changed (due to noise or pressing)
    if (reading != lastButtonState) {
        // Reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // If the button state has changed
        if (reading != buttonState) {
            buttonState = reading;

            // If the new button state is LOW (button pressed)
            if (buttonState == LOW) {
                if (alarm_active) {
                    alarm_active = false;
                    digitalWrite(ALARM_PIN, LOW);
                } else if (counting) {
                    counting = false;
                    currentLED = 0;
                } else {
                    currentLED = (currentLED + 1) % 16; // Change to 16 to handle numbers from 0 to 15
                }
                delay(50); // Small delay to avoid multiple detections of a single press
            }
        }
    }

    // Save the current button state
    lastButtonState = reading;
}

void start_timer() {
    if (currentLED == 0) return; // Do not start the timer if currentLED is 0
    counting = true;
    countdownStartTime = millis();
}

void display_number(int number) {
    for (int i = 0; i < 100; i++) { // Repeat to make the LEDs appear lit simultaneously
        if (number & 0x01) charliePlexPin(0);
        if (number & 0x02) charliePlexPin(1);
        if (number & 0x04) charliePlexPin(2);
        if (number & 0x08) charliePlexPin(3);
        if (number == 0) charliePlexPin(-1); // Turn off all LEDs
        _delay_us(100); // Short delay to quickly switch between LEDs
    }
}

void charliePlexPin(int led) {
    // Set pins as inputs
    pinMode(LED_PIN1, INPUT);
    pinMode(LED_PIN2, INPUT);
    pinMode(LED_PIN3, INPUT);
    digitalWrite(LED_PIN1, LOW);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);

    switch (led) {
        case -1:
            // Turn off all LEDs
            pinMode(LED_PIN1, INPUT);
            pinMode(LED_PIN2, INPUT);
            pinMode(LED_PIN3, INPUT);
            break;
        case 0:
            pinMode(LED_PIN1, OUTPUT);
            pinMode(LED_PIN2, OUTPUT);
            digitalWrite(LED_PIN1, LOW);
            digitalWrite(LED_PIN2, HIGH);
            break;
        case 1:
            pinMode(LED_PIN1, OUTPUT);
            pinMode(LED_PIN2, OUTPUT);
            digitalWrite(LED_PIN1, HIGH);
            digitalWrite(LED_PIN2, LOW);
            break;
        case 2:
            pinMode(LED_PIN2, OUTPUT);
            pinMode(LED_PIN3, OUTPUT);
            digitalWrite(LED_PIN2, LOW);
            digitalWrite(LED_PIN3, HIGH);
            break;
        case 3:
            pinMode(LED_PIN2, OUTPUT);
            pinMode(LED_PIN3, OUTPUT);
            digitalWrite(LED_PIN2, HIGH);
            digitalWrite(LED_PIN3, LOW);
            break;
    }
}
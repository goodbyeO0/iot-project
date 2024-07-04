#define enB 10
#define in3 8
#define in4 9
int motorSpeedB = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(enB, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
}

void loop()
{
    motorSpeedB = 255;
    Serial.println(motorSpeedB);   // Print the value of motorSpeedA
    analogWrite(enB, motorSpeedB); // Send PWM signal to motor A
    // Set Motor A backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}

#define enB 27
#define in3 26
#define in4 25

void setup()
{
    Serial.begin(9600);
    pinMode(enB, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
}

void loop()
{
    int motorSpeedB = 255;
    Serial.println(motorSpeedB);   // Print the value of motorSpeedA
    analogWrite(enB, motorSpeedB); // Send PWM signal to motor A
    // Set Motor A backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    delay(1000);
}

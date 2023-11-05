void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.write("const char *str");
  Serial1.begin(38400);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    Serial1.write(Serial.read());
    // String input = "\n"+Serial.readString();
    // char buff[input.length()+1];
    // input.toCharArray(buff, input.length()+1);
    // Serial.write(buff);
  }

  if(Serial1.available() > 0){
    Serial.write(Serial1.read());
  }
}

bool flag = false;
int first=0, second=0, third=0, fourth=0, fifth=0, i=0;

void setup() {
  pinMode(4, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available()) {
    char data[30];
    int amount = Serial.readBytesUntil(';', data, 30);
    data[amount] = NULL;
    char *rest = NULL;
    char *token;
    String st = "";
    for (token = strtok_r(data, ",", &rest), i=0;token!=NULL;token=strtok_r(NULL, ",", &rest), i++) {
      
      st = String(token);
      switch(i) {
        case 0 : {
          first = st.toInt();
          break;
        }
        case 1 : {
          second = st.toInt();
          break;
        }
        case 2 : {
          third = st.toInt();
          break;
        }
        case 3 : {
          fourth = st.toInt();
          break;
        }
        case 4 : {
          fifth = st.toInt();  
          break;
        }
      }
    }

    Serial.println(first);
    Serial.println(second);
    Serial.println(third);
    Serial.println(fourth);
    Serial.println(fifth);

  }

}

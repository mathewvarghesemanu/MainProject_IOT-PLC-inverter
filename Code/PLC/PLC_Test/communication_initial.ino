#include <SoftwareSerial.h>

#define MESSAGE_BUFFER_COUNT 20
#define FRAME_LENGTH 10
#define TIMEOUT 5000

//=============Control signals==========================//
#define _REQUEST 0
#define _REPLY 1
#define _ERROR 2


//==============Node Properties========================//

#define NODE_ID 2
#define MASTER_NODE 1

//Available nodes in the circuit
const byte nodes[]={1,2};

//subnodes available
static const uint8_t subNodes[]={1,2,3};
static const uint8_t subNodePins[]={5,4,3}; //relay
static const uint8_t subNodeSensors[]={14,15,16};  // A0, A1, A2



//========== Variable Declarations =====================//

SoftwareSerial mySerial(11, 10); // RX, TX
int i;
byte state;

struct byteArray {
    byte byteAt[FRAME_LENGTH];
};

//Channel flags
uint8_t channelStatus; //0=free to transmit , 1=busy

//Node flags
uint8_t nodeStatus; //0=waiting for data , 1=waiting for reply


 


//=======Function definitions and declarations=======//
byteArray readByteArray(){
  long prevmillis;
  byteArray bytes;
  while(state==1){
      if (mySerial.available()) {
          if(mySerial.read()==170){
            prevmillis=millis();
            for(i=0;(i<FRAME_LENGTH);i++){
              
              while(!mySerial.available()){
                if(millis()-prevmillis > TIMEOUT){
                  for(i=0;(i<FRAME_LENGTH);i++){
                    bytes.byteAt[i]=1;
                    }
                   return bytes;
                  }
                };
                
              bytes.byteAt[i]=mySerial.read();
              prevmillis=millis();
            }
            if(i==FRAME_LENGTH){
              return bytes;
              }
          }
      }
    }
  }

uint16_t fletcher16(const byte *data, size_t len){
        uint32_t c0, c1;
        unsigned int i;

        for (c0 = c1 = 0; len >= 5802; len -= 5802) {
                for (i = 0; i < 5802; ++i) {
                        c0 = c0 + *data++;
                        c1 = c1 + c0;
                }
                c0 = c0 % 255;
                c1 = c1 % 255;
        }
        for (i = 0; i < len; ++i) {
                c0 = c0 + *data++;
                c1 = c1 + c0;
        }
        c0 = c0 % 255;
        c1 = c1 % 255;
        return (c1 << 8 | c0);
}


boolean sentDopis(byte id,byte fromNode,byte fromSubNode,byte control,uint16_t value, byte toNode,byte toSubNode){
  if(channelStatus==0){
    byte msgframe[FRAME_LENGTH+1]={170,id,fromNode,fromSubNode,control,(byte)(value>>8),(byte)(value & 255),toNode,toSubNode,0,0};
    uint16_t checksum=fletcher16(msgframe+1,8);
    msgframe[FRAME_LENGTH-1]=checksum>>8;
    msgframe[FRAME_LENGTH]=checksum&255;
    for(int i=0;i<FRAME_LENGTH+1;i++){
      mySerial.write(msgframe[i]);
    }
    return true;
    }
  else{
    return false;
    }
  }

byte *byteArrayToArray(byteArray a){
    static byte msg[FRAME_LENGTH];
    for(i=0;(i<FRAME_LENGTH);i++){
       msg[i]=a.byteAt[i];
      }
    return msg;
  }



//========== Class Definitions =========================//
//class for the letter service through a single postman
class dopis{
  private:
    byte msgframe[FRAME_LENGTH];
    byte id,fromNode,fromSubNode,control,toNode,toSubNode;
    uint8_t k1,k0;
    uint16_t value;
    long timestamp;
    uint16_t checksum;
    boolean ack;


  public:
  void parseMsg(){
    id=msgframe[0];
    fromNode=msgframe[1];
    fromSubNode=msgframe[2];
    control=msgframe[3];
    toNode=msgframe[6];
    toSubNode=msgframe[7];
    k1=msgframe[8];
    k0=msgframe[9];
    value=msgframe[4]<<8 | msgframe[5];
    }
    
  dopis(){
   ack=false;
  }
  
  dopis(byte *msg){
    for(int i=0;i<FRAME_LENGTH;i++){
      msgframe[i]=*(msg++);
    }
    timestamp=millis();
    parseMsg();
    ack=false;
  }
  
  void reAassign(byte *msg){
    for(int i=0;i<FRAME_LENGTH;i++){
      msgframe[i]=*(msg++);
    }
    timestamp=millis();
    parseMsg();
    ack=false;
  }
  
  boolean validate(){
    checksum=fletcher16(msgframe,FRAME_LENGTH-2);
    if((k0==(uint8_t)(checksum & 255)) && (k1==(uint8_t)(checksum>>8))){
      return true;
    }else{
      return false;
      }
    }

  boolean existsInArray(const byte *data,const byte element,size_t len){
    for(int i=0;i<len;i++){
      if((*(data++)-element)==0){
        return true;
        }
      }
    return false;
    }
    
  boolean process(){
      if(validate()){
        if(toNode==NODE_ID){
          if(existsInArray(subNodes,toSubNode,sizeof(subNodes)/sizeof(subNodes[0])))
            {
              
            }
          }
        else{
          return 0;
          }
        }
      else{
        return 0;
        }  
        
      
      }
    
};




//========== Setup Function =====================//
void setup() {
 Serial.begin(9600);
 mySerial.begin(9600);
 
}

//========== Main Function =====================//

void loop() {
 state=1;
  
   byteArray input=readByteArray(); 
  
   for(i=0;(i<FRAME_LENGTH);i++){
    Serial.print(input.byteAt[i]);
    Serial.print(" ");
    }
    Serial.print(" --> ");
 
dopis dopis1(byteArrayToArray(input));

Serial.println(dopis1.validate());

}

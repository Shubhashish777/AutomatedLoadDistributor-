#include <stdlib.h>

const byte PINS[5] = {A0,A1,A2,A3,A4};
int sub=0;
const byte outputpin[5]={3,4,5,6,7};
 byte priority[5]={20,10,30,50,40};
 int prior[5]={20,10,30,50,40};
//float analogread[5];
float current[5];
float watt_hours[5];
//float maxima[5];
//float minima[6];
int power[5];
int samples=3000;
int tresh[5]={25,50,100,150,200};
//int treshhold=200;
int old=0;


struct loadobj
{
   int pin;
   byte val;
   char pinname[];
}; 

byte K[6][201];

void knapsack_and_backtrack(int W, int wt[], byte val[], byte n)

{ 
 
    byte i=0;
    int w=0; 
  
    for (i = 0; i <= n; i++) { 
        for (w = 0; w <= W; w++) { 
            if (i == 0 || w == 0) 
                K[i][w] = 0; 
            else if (wt[i - 1] <= w) 
                K[i][w] = max(val[i - 1] +K[i - 1][w - wt[i - 1]], K[i - 1][w]); 
            else
                K[i][w] = K[i - 1][w]; 
        } 
    } 
 
    w = W; 
    int res = K[n][W];
    byte cop[5]={1,1,1,1,1};
    for (i = n; i > 0 && res > 0; i--) { 
        if (res == K[i - 1][w])  
            //{digitalWrite( i-1+3,LOW);}  
            continue;    
        else { 
             //   printf("%d ", wt[i - 1]); 
     
           if(wt[i-1]!=0) 
            {Serial.println("Allowed");
            Serial.println(String(wt[i-1])+"  "+"L"+String(i));}
            //digitalWrite( i-1+3, HIGH);
            cop[i-1]=0;
            res = res - val[i - 1]; 
            w = w - wt[i - 1]; 
        } 
    } 

    for (int j=0; j<5; j++)
    {

      if (cop[j]!=0)
      {
        Serial.println("OFF L"+String(j+1));
        digitalWrite( j+3, LOW);
      }
    }

    
} 


void findpeaks()
{
     for (int i=0; i<5; i++)
    {
         int count=0;
         float low=1025;
         float high=-10;
         while (count<=samples)
         {

            float temp=analogRead(PINS[i]);
            
            if (temp>high)
            {
              high=temp;   
            }
            if(temp<low)
            {
              low=temp;
            }
            count++;
     
         }
         //maxima[i]=high;
         //minima[i]=low;
         float analogread=high-low;
         current[i]=((analogread * (5.0 / 1024.0)) )/0.066; 
         current[i]=0.353*current[i];
         
         
    }
}



void setup() 
{
  Serial.begin(9600);

  for (int k=3; k<8; k++)
  {
    pinMode(k,OUTPUT);
    digitalWrite( k, HIGH);
  }

}

int cmp( const void * a, const void * b)
{
    const loadobj *A = (loadobj *)a;
    const loadobj *B = (loadobj *)b;
   if(A->val<B->val)
  {
     return -1;
  }
  else if (A->val>B->val)
  return 1;
  else
  {
    return 0;
  }
}

void loop() 
{
    
    //ccurrent();
    float vrms=(0.707*5);
    
    int t = millis()/1000;
    int check=t/7;
    int dum=check%5;
    int treshhold=tresh[dum];

    if (old!=dum)
    {
      
    for (int ko=3; ko<8; ko++)
    {

    digitalWrite( ko, HIGH);
    }
      old=dum;
    }
    findpeaks();
    
    float total_power=0.00;
    int dummy=0;
    struct loadobj* obj = malloc(5 * sizeof(struct loadobj));
    Serial.println("TRESHHOLD IS: "+String(treshhold));
    Serial.println("LOADS ON THE BASIS OF PRIORITY\n");
    

    //Serial.println("Watt_hours  Load   Power   time  Current");

    for (int i=0; i<5;i++)
    {
        obj[i].pin=i+1;
        obj[i].val=prior[i];
        obj[i].pinname[0]="L";
        obj[i].pinname[1]=(i+1)+ '0';

    
    }

     qsort(obj, 5, sizeof(struct loadobj), cmp);

      Serial.println("L"+String(obj[0].pin)+"<"+"L"+String(obj[1].pin)+"<"+"L"+String(obj[2].pin)+"<"+"L"+String(obj[3].pin)+"<"+"L"+String(obj[4].pin));
      //Serial.println("L"+String(obj[0].val)+"<"+"L"+String(obj[1].val)+"<"+"L"+String(obj[2].val)+"<"+"L"+String(obj[3].val)+"<"+"L"+String(obj[4].val));
      free(obj);
  
    
    
    
   
   Serial.println("WH    Load   Power   time  Current");
    for (int i=0; i<5; i++)
    {
      //Serial.println(String(current[i])+"    "+String(i)+"   "+String(vrms*current[i]));
      
      power[i]=(vrms*current[i]);
      total_power+=(vrms*current[i]);
      dummy+=power[i];
      watt_hours[i]=watt_hours[i]+(((vrms*current[i])*(t-sub))/3600);

      Serial.println(String(watt_hours[i])+"    L"+String(i+1)+"   "+String(vrms*current[i])+"   "+String(t)+"  "+String(current[i]));

    }
    sub=t;
    
    //Serial.println(String(total_power)+"   "+String(dummy));

    
   
    
    delay(100);
    
    if (total_power>treshhold)
    {
      
      knapsack_and_backtrack(treshhold, power, priority, 5);
      
    }
    else
    {
      Serial.println("ALL LOADS ARE ON. Total power:  "+String(total_power));
    }

    delay(100);
    
}

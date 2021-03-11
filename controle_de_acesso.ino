//Código de controle de acesso (Com Arduino).
//Autor: Pedro Lins, graduando de engenharia elétrica pela UFRPE.

//bibliotecas
#include <SPI.h> //biblioteca para comunicação spi
#include <MFRC522.h> //biblioteca para o uso do leitor RFID
// portas
// Utilização da diretiva de compilação #define no lugar de varáveis, justamente para economizar mais memória no nosso microcontrolador
#define led_vermelho 6 
#define led_azul 7
#define relay 5
#define buzzer 8
#define pino_sda 10
#define pino_rst 9 
//variáveis
String id_tag = ""; // Está vai ser a variável que vai armazenar o a identificação de cada tag lida pelo leitor RFID.
bool permissao = false; //Está variável vai verificar a permissão, se a tag lida pelo leito RFID pode ter acesso ou não

// Está será a matriz de strings responsavél por armazenar as ID's das tag's cadastradas
String tags_cadastradas[] = {"79b9ec2","39728bc1","id3"};

// Agora vamos criar um objeto do tipo MFRC522 e passar o pino do sda e do rst como parâmetros
MFRC522 RFID(pino_sda,pino_rst);

void setup() {
  //acionado as portas dos led's, relay e buzzer com o laço FOR
   for (int i = 5; i <= 8;i++) {
    pinMode(i,OUTPUT);
   }
   Serial.begin(9600); // inicializando a Serial para obter as informações dos ID's 
   SPI.begin(); // inicializando a comunição SPI
   RFID.PCD_Init(); //Irá inicializar o leitor
   digitalWrite(relay,HIGH); //para inicializar com o relay desacionado
}

void loop() {
  leitura_do_rfid();
}
// Função que irá exercer a leitura das tags 
void leitura_do_rfid() {
  id_tag = ""; // sempre deve coomeçar a leitura com a id_tag vazia
  //A condição abaixo verifica se existe alguma tag sendo aproximada
  if (!RFID.PICC_IsNewCardPresent() || !RFID.PICC_ReadCardSerial()){
    delay(10);
    return;
  }
  // O laço for abaixo vai armazenar o id da tag lida pelo RIFD na nossa variável, utilizando a função RFID.uid
  for (int i = 0; i < RFID.uid.size; i++) {
    id_tag.concat(String(RFID.uid.uidByte[i], HEX));
  }
  //O laço for abaixo vai comparar se o id lido pelo RFID está cadastrado para liberar o acesso ou não
  for (int i = 0; i < (sizeof(tags_cadastradas)/sizeof(String)); i++) {
    if (id_tag.equalsIgnoreCase(tags_cadastradas[i])) { 
      //Caso a condicional seja verdadeira a permissao será concedida
      permissao = true; 
    }
  }
  //Condicionais que vão chamar as funções que permitem ou não o acesso da tag
  if (permissao == true) {
    permissao_concedida(); //função que aciona o relay, leds e buzzer, mostrando um feedback sonoro e visual
  }else { 
    permissao_nao_concedida(); //função que da um feedback sonoro e visual de que a tag em questão não tem acesso
  }
  delay(1500); // tempo para nova leitura
}

// Função exercida quando a tag está cadastrada 
void permissao_concedida() {
  Serial.println("tag cadastrada: " + id_tag); //Mostra o id da tag cadastrada
  for (int i = 0; i < 2; i++) {//Laço for utilizado para dar dois loops 
    tone(buzzer, 500);//Aciona o buzzer na freguência de 500Hz(mais grave)
    digitalWrite(led_azul,HIGH);//Liga o led azul 
    delay(125);//tempo de duração do buzzer e do led ligados 
  
    noTone(buzzer); // delisga o buzzer
    digitalWrite(led_azul,LOW);//desliga o led 
    delay(125);//tempo que o buzzer e led ficam desligados
  }
  digitalWrite(relay,LOW);// aciona o relay
  delay(2000);// tempo de acionamento do relay 
  digitalWrite(relay,HIGH);//desaciona o relay
  permissao = false;// volta a permissao para false, para uma nova leitura
}
//Função exercida quando a tag não é reconhecida no cadastro
void permissao_nao_concedida() {
  Serial.println("Tag não cadastrda: " + id_tag); // mostra o id da tag não cadastrada
  tone(buzzer, 1000); //aciona o buzzer na freguência de 1000Hz(mais agudo)
  digitalWrite(relay,HIGH);//desaciona o relay
  digitalWrite(led_vermelho,HIGH);//liga o led vermelho
  delay(1000);//tempo de duração do led e do buzzer ligados 
  digitalWrite(led_vermelho,LOW);//desliga o led vermelho
  noTone(buzzer);//desaciona o buzeer 
}

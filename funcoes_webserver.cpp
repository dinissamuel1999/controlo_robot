#include "funcoes_webserver.h"

struct tipo_dados_queue_web_brain_cmds {
	String tipo_controlo;
	int valor;
};

const char *htmlHomePage PROGMEM
		= R"HTMLHOMEPAGE(
<!DOCTYPE html>
    <html>
    <head>
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
      <style>
      .arrows {
        font-size:30px;
        color:green;
      }
      td.button {
        background-color:black;
        border-radius:25%;
        box-shadow: 5px 5px #888888;
      }
      td.button:active {
        transform: translate(5px,5px);
        box-shadow: none; 
      }

      .noselect {
        -webkit-touch-callout: none; /* iOS Safari */
          -webkit-user-select: none; /* Safari */
          -khtml-user-select: none; /* Konqueror HTML */
            -moz-user-select: none; /* Firefox */
              -ms-user-select: none; /* Internet Explorer/Edge */
                  user-select: none; /* Non-prefixed version, currently
                                        supported by Chrome and Opera */
      }

      .slidecontainer {
        width: 100%;
      }

      .slider {
        -webkit-appearance: none;
        width: 100%;
        height: 15px;
        border-radius: 5px;
        background: #d3d3d3;
        outline: none;
        opacity: 0.7;
        -webkit-transition: .2s;
        transition: opacity .2s;
      }

      .slider:hover {
        opacity: 1;
      }
    
      .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 25px;
        height: 25px;
        border-radius: 50%;
        background: red;
        cursor: pointer;
      }

      .slider::-moz-range-thumb {
        width: 25px;
        height: 25px;
        border-radius: 50%;
        background: red;
        cursor: pointer;
      }

      </style>
    
    </head>
    <body class="noselect" align="center" style="background-color:white">
      <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
        <tr>
          <img id="cameraImage" src="http://192.168.1.10:81/stream" style="width:400px;height:250px"></td>
        </tr>
        <div class="content">
          <div class="card">
            <h2>Variaveis</h2>
            <p class="var_velocidade">Velocidade m/s: <span id="var_velocidade_ms">%VAR_VELOCIDADE_MS%</span></p>
            <p class="var_velocidade">Velocidade km/h: <span id="var_velocidade_km">%VAR_VELOCIDADE_KM%</span></p>
          </div>
          <div class="card">
            <p class="var_distancia">Distancia: <span id="var_distancia">%VAR_DISTANCIA%</span></p>
          </div>
        <tr>
          <td></td>
          <td class="button" ontouchstart='sendButtonInput("MoveCar","1")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8679;</span></td>
          <td></td>
        </tr>
        <tr>
          <td class="button" ontouchstart='sendButtonInput("MoveCar","3")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8678;</span></td>
          <td class="button"></td>    
          <td class="button" ontouchstart='sendButtonInput("MoveCar","4")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8680;</span></td>
        </tr>
        <tr>
          <td></td>
          <td class="button" ontouchstart='sendButtonInput("MoveCar","2")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8681;</span></td>
          <td></td>
        </tr>
        <tr/><tr/>
              
          <tr>
          <td style="text-align:left"><b>Luz:</b></td>
          <td colspan=2>
            <div class="slidecontainer">
              <input type="range" min="0" max="255" value="0" class="slider" id="Light" oninput='sendButtonInput("Light",value)'>
            </div>
          </td>   
        </tr>
        
      </table>
    
      <script>
        var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";      
        var websocketCarInput;
        window.addEventListener('load', onLoad);
      

      function onLoad(event) {
          initWebSocket();
          //initButton();
      } 

      
      
      
      
      function initWebSocket() 
      {
        //initCameraWebSocket ();
        initCarInputWebSocket();
      }

        function initCarInputWebSocket() 
        {
          websocketCarInput = new WebSocket(webSocketCarInputUrl);
          websocketCarInput.onopen    = function(event)
          {
            console.log("[open] Connection established"); 
           // sendButtonInput("Speed", document.getElementById("Speed").value);
            //sendButtonInput("Light", document.getElementById("Light").value);
            //sendButtonInput("Pan", document.getElementById("Pan").value);
            //sendButtonInput("Tilt", document.getElementById("Tilt").value);                    
          };
          //websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};
          websocketCarInput.onclose   = function(event){};
          websocketCarInput.onmessage = onMessage; 
        }
        
      

        function sendButtonInput(key, value) 
        {
          console.log("estou dentro do send button"); 
          var data = key + "," + value + '}' +'\0';
          websocketCarInput.send(data);
        }
        
        var appEvents = {
            down: 'ontouchstart' in window ? 'touchstart' : 'click' /* ou mousedown */,
            move: 'ontouchmove' in window ? 'touchmove' : 'mousemove',
            up: 'ontouchend' in window ? 'touchend' : 'mouseup'
            // e outros que precises
        }
        

       /* function onMessage(event) {
          var velocidade = 25;
          if (event.data == "1"){
          }
          else{
          }
          document.getElementById('var_velocidade').innerHTML = state;
        }*/


        function onMessage(event) {
            let data = JSON.parse(event.data);
            document.getElementById("var_velocidade_ms").innerHTML    = data.velocidade;
            document.getElementById("var_velocidade_km").innerHTML    = (data.velocidade * 3.6).toFixed(2);
            document.getElementById("var_distancia").innerHTML    = data.distancia;
        
          }

    
        //window.onload = initWebSocket;
        document.getElementById("mainTable").addEventListener("touchend", function(velocidade){
          event.preventDefault()
        });      
      </script>
    </body>    
  </html>
  
)HTMLHOMEPAGE";

void handleRoot(AsyncWebServerRequest *request) {
	request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server,
		AsyncWebSocketClient *client, AwsEventType type, void *arg,
		uint8_t *data, size_t len) {

	tipo_dados_queue_web_brain_cmds dados_recebidos;
	switch (type) {
	case WS_EVT_CONNECT:
		//Serial.printf("WebSocket client #%u connected from %s\n", client->id(),client->remoteIP().toString().c_str());
		break;
	case WS_EVT_DISCONNECT:
		//Serial.printf("WebSocket client #%u disconnected\n", client->id());
		dados_recebidos.tipo_controlo = "MoveCar";
		dados_recebidos.valor = CODIGO_WEB_BRAIN_PARAR;
		xQueueOverwrite(xQueue_web_brain_cmds_controlo_veiculo,
				(void* ) &dados_recebidos);

		break;
	case WS_EVT_DATA:
		AwsFrameInfo *info;
		info = (AwsFrameInfo*) arg;
		if (info->final && info->index == 0 && info->len == len
				&& info->opcode == WS_TEXT) {
			String key;
			int value;

			String str = (char*) data;
			char aux[9];
			uint8_t i = 0, j = 0;

			while (j < str.length()) {
				if (str.charAt(j) == ',') {
					aux[i] = '\0';
					key = aux;
					i = 0;
				} else if (str.charAt(j) == '}') {
					aux[i] = '\0';
					value = atoi(aux);
					i = 0;
				} else {
					aux[i] = (char) str.charAt(j);
					i++;
				}
				j++;
			}
			dados_recebidos.tipo_controlo = key;
			dados_recebidos.valor = value;
			xQueueOverwrite(xQueue_web_brain_cmds_controlo_veiculo,
					(void* ) &dados_recebidos);
		}
		break;
	case WS_EVT_PONG:
	case WS_EVT_ERROR:
		break;
	default:
		break;
	}
}







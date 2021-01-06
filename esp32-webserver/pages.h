const char MAIN_PAGE[] = R"=====(
<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css"
        integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">

    <style>
        .btn {
            width: 100%;
            margin: 5px;
        }

        .my-text {
            font-size: 20px;
            padding: 10px;
        }

        .my-div {
            margin-top: 15px;
            
        }
    </style>

    <title>ESP32 WEB SERVER</title>
</head>

<body>
    <div class="container">
        <div class="row">
            <div class="col-md-12 col-xs-12">
                <h1 class="text-center">ESP32 WEB SERVER AJAX EXAMPLE</h1>
            </div>
        </div>

        <hr>

        <div class="row">
            <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">LED 1 TOGGLE</h2>
                </div>
                <div class="col-md-6 col-xs-6">
                    <button onclick="sendData(0, 1)" type="button" class="btn btn-primary">ON</button>
                </div>
                <div class="col-md-6 col-xs-6">
                    <button onclick="sendData(0, 0)" type="button" class="btn btn-warning">OFF</button>
                </div>
                <div class="col-md-12 col-xs-12 text-center my-text">
                    LED Status: <span id="ledStat">OFF</span>
                </div>

                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">LED 2 TOGGLE</h2>
                </div>
                <div class="col-md-6 col-xs-6">
                    <button onclick="sendData(1, 1)" type="button" class="btn btn-primary">ON</button>
                </div>
                <div class="col-md-6 col-xs-6">
                    <button onclick="sendData(1, 0)" type="button" class="btn btn-warning">OFF</button>
                </div>
                <div class="col-md-12 col-xs-12 text-center my-text">
                    LED Status: <span id="ledStat">OFF</span>
                </div>
            </div>

            <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">ADC VALUE</h2>
                </div>
                <div class="col-md-12 col-sm-12 text-center my-text">
                    ADC Value: <span id="adcVal">---</span>
                </div>
            </div>

            <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">RGB LED</h2>
                </div>
                <div class="row">
                    <div class="col-md-8 col-xs-8 my-div">
                        <input id="r" type="range" min="0" max="100" value="0" onchange="sendRGB()">
                    </div>
                    <div class="col-md-4 col-xs-4 my-div text-center">
                        <span id="rVal">0</span>
                    </div>
                </div>
                <div class="row">
                    <div class="col-md-8 col-xs-8 my-div">
                        <input id="g" type="range" min="0" max="100" value="0" onchange="sendRGB()">
                    </div>
                    <div class="col-md-4 col-xs-4 my-div text-center">
                        <span id="gVal">0</span>
                    </div>
                </div>
                <div class="row">
                    <div class="col-md-8 col-xs-8 my-div">
                        <input id="b" type="range" min="0" max="100" value="0" onchange="sendRGB()">
                    </div>
                    <div class="col-md-4 col-xs-4 my-div text-center">
                        <span id="bVal">0</span>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <footer class="container footer">
        <div class="col-md-12 text-center">
            <h2>Oğuzhan BAŞER</h2>
            <h3><a href="http://www.elektrobot.net">elektrobot.net</a></h3>
            <h3><a href="https://www.instagram.com/elektrobot.elektronik/">Instagram</a></h3>
            <br>
        </div>
    </footer>

    <script>

        function sendRGB() {
            //get rgb values from sliders
            var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;
            var blueVal = document.getElementById('b').value;

            // convert rgb values range from 0 - 100 to 0 - 255
            var r = parseInt(redVal / 100.0 * 255).toString();
            var g = parseInt(greenVal / 100.0 * 255).toString();
            var b = parseInt(blueVal / 100.0 * 255).toString();

            // show slider value in labels
            document.getElementById('rVal').innerHTML = String(redVal);
            document.getElementById('gVal').innerHTML = String(greenVal);
            document.getElementById('bVal').innerHTML = String(blueVal);

            // wait for answer callback
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };

            // create querrystring
            var sendStr = "setRGB?r=" + r + "&g=" + g + "&b=" + b;
            // console.log(sendStr);

            // send querrystring using xmlhttprequest
            xhttp.open("GET", sendStr, true);
            xhttp.send();
        }

        function sendData(ledNum, led) {
            var xhttp = new XMLHttpRequest();

            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("ledStat").innerHTML =
                        this.responseText;
                }
            };
            xhttp.open("GET", "setLED?LEDstate=" + led + "&ledNum=" + ledNum, true);
            xhttp.send();
        }

        setInterval(function () {
            // Call a function repetatively with 2 Second interval
            getData();
        }, 1000); //2000mSeconds update rate

        function getData() {
            
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("adcVal").innerHTML =
                        this.responseText;
                    console.log(this.responseText);
                }
            };
            xhttp.open("GET", "readADC", true);
            xhttp.send();
        }
    </script>

    <script src="https://code.jquery.com/jquery-3.4.1.slim.min.js"
        integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n"
        crossorigin="anonymous"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"
        integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa"
        crossorigin="anonymous"></script>
</body>

</html>
)=====";

const char PAGE_404[] = R"====(
<html>
    <head>
        <title>404</title>
    </head>
    <body>
        <center><h1>ERROR CODE 404</h1></center>
        <center><h1>REQUESTED PAGE NOT FOUND</h1></center>
    </body>
</html>
)====";
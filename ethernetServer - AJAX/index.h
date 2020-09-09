const char header_plain[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

const char header_html[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

const char myHtml[] PROGMEM = R"=====(
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

    <title>W5500 WEB SERVER</title>
</head>

<body>
    <div class="container">
        <div class="row">
            <div class="col-md-12 col-xs-12">
                <h1 class="text-center">W5500 WEB SERVER AJAX EXAMPLE</h1>
            </div>
        </div>

        <hr>

        <div class="row">
            <div class="col-md-4 col-xs-12">
                <div class="col-md-12 col-xs-12">
                    <h2 class="text-center">LED TOGGLE</h2>
                </div>
                <div class="col-md-6 col-xs-6">
                    <button onclick="sendData(1)" type="button" class="btn btn-primary">ON</button>
                </div>
                <div class="col-md-6 col-xs-6">
                    <button onclick="sendData(0)" type="button" class="btn btn-warning">OFF</button>
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
        var xhttp = new XMLHttpRequest();

        function sendRGB() {
            var redVal = document.getElementById('r').value;
            var greenVal = document.getElementById('g').value;
            var blueVal = document.getElementById('b').value;
            var r = parseInt(redVal / 100.0 * 255).toString(16);
            var g = parseInt(greenVal / 100.0 * 255).toString(16);
            var b = parseInt(blueVal / 100.0 * 255).toString(16);

            document.getElementById('rVal').innerHTML = String(redVal);
            document.getElementById('gVal').innerHTML = String(greenVal);
            document.getElementById('bVal').innerHTML = String(blueVal);

            if (r.length < 2) { r = '0' + r; }
            if (g.length < 2) { g = '0' + g; }
            if (b.length < 2) { b = '0' + b; }
            var rgb = r + g + b;
            console.log('RGB: ' + rgb);
            sendRGBData(rgb);
        }

        function sendRGBData(ledCode) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    
                }
            };
            xhttp.open("GET", "setRGB?color=" + ledCode, true);
            xhttp.send();
        }

        function sendData(led) {
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("ledStat").innerHTML =
                        this.responseText;
                }
            };
            xhttp.open("GET", "setLED?LEDstate=" + led, true);
            xhttp.send();
        }
        // sendData(0);

        setInterval(function () {
            // Call a function repetatively with 2 Second interval
            getData();
        }, 500); //2000mSeconds update rate

        function getData() {
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("adcVal").innerHTML =
                        this.responseText;
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

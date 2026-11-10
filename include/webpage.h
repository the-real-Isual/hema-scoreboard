#pragma once

#include <Arduino.h>

static const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Counter</title>

<style>
  body {
    font-family: Arial, sans-serif;
    background: #20252b;
    color: white;
    text-align: center;
    margin: 0;
    padding-top: 60px;
  }

  .container {
    max-width: 300px;
    margin: auto;
    padding: 30px;
    background: #303841;
    border-radius: 20px;
  }

  #counter {
    font-size: 90px;
    font-weight: bold;
    margin: 20px 0;
  }

  button {
    width: 120px;
    height: 65px;
    font-size: 35px;
    margin: 10px;
    border: none;
    border-radius: 12px;
    cursor: pointer;
    color: white;
    background: #008cba;
  }

  button:active {
    transform: scale(0.95);
  }

  .reset {
    width: 260px;
    height: 50px;
    font-size: 20px;
    background: #d9534f;
    margin-top: 25px;
  }
</style>
</head>

<body>

<div class="container">

  <h2>Fechter Rot</h2>

  <div id="counter">00</div>

  <div>
    <button onclick="changeCounter(1)">&#8593;</button>
    <button onclick="changeCounter(-1)">&#8595;</button>
  </div>

  <button class="reset" onclick="resetCounter()">RESET</button>

</div>

<script>

let counter = 0;

function updateDisplay() {
  document.getElementById("counter").innerText =
    String(counter).padStart(2, '0');
}

async function sendCounter(value) {
  try {
    const response = await fetch('/counter', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
      },
      body: 'value=' + value
    });

    if (!response.ok) {
      throw new Error('HTTP ' + response.status);
    }

    const result = await response.json();
    counter = result.value;
    updateDisplay();

  } catch (error) {
    console.error('Fehler:', error);
  }
}

function changeCounter(amount) {
  let newValue = (counter + amount + 100) % 100;
  sendCounter(newValue);
}

function resetCounter() {
  sendCounter(0);
}

</script>

</body>
</html>
)rawliteral";

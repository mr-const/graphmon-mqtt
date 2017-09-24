# Graphmon-MQTT

## NVidia Grpahics Card status report agent using lightweight mqtt protocol [Windows/Linux]

Agent reports current temperature, gpu and memory frequencies, power consumption, fan speed to a mqtt server (supports SSL)
All parameters are defined in a simple `config.json` file, which should be placed to the working directory of an agent
```
{
	"connectionString": "tcp://raspberrypi.lan:1883",
	"clientId": "Graphmon",
	"telemetryInterval": 10,
	"mqttTopicName": "homepc",
	"mqttMessageTimeout": 10
}
```

Reported topic will look like `/tele/homepc/gpu0`
The payload will look like this:
```
{
	"fanSpeed": 36,
	"graphClock": 1594, 
	"memClock":4404, 
	"name": "GeForce GTX 1070",
	"power": 122593, 
	"temperature": 57
}

```

## Building

NB: Use cmake for project/makefile generation

### All platforms

1. git clone
2. git submodule init
3. git submodule update

### Windows Build

* Install OpenSSL to `C:/OpenSSL-Win64` which could be obtained here: http://www.slproweb.com/products/Win32OpenSSL.html


### Linux Build

Ubuntu (didn't test on other flavors, but don't think it'll be problematic)

* apt-get install libboost-all-dev libssl-dev
(unfortunately boost is required for cpprestsdk)

This application uses customly built paho-mqtt-c library (src/vendor/pahomqtt/lib/, which should be placed to /usr/lib). Maybe, when I'll implement 'make install` it won't be necessary.
<a href="https://www.bigclown.com/"><img src="https://bigclown.sirv.com/logo.png" width="200" alt="BigClown Logo" align="right"></a>

# BigClown Ethernet Climate Monitor

This repository contains firmware for [Core Module](https://shop.bigclown.com/core-module).

This example creates a MQTT client that periodically sends temperature, humidity, pressure and luminosity data to the configured MQTT broker. It can be very easily extended with CO2 measurement with CO2 Module.

In addition it also sends MQTT message on button press, so you can use it as a MQTT ethernet button, and also you can control Relay Module and Core Module's LED over MQTT.

[MQTT topics are the same](https://developers.bigclown.com/interfaces/mqtt-topics) like for 868 MHz Radio Dongle, so you can easily use wireless and wired devices the same way.

Code contains simple webserver which works, but the webpage is not showing measured data yet.

```
node/ethernet-node/thermometer/0:1/temperature 28.250000
node/ethernet-node/hygrometer/0:4/relative-humidity 51.392120
node/ethernet-node/barometer/0:0/pressure 98466.500000
node/ethernet-node/thermometer/0:1/temperature 28.312500
node/ethernet-node/hygrometer/0:4/relative-humidity 50.949615
node/ethernet-node/lux-meter/0:0/illuminance 316.799988
```

The Ethernet Module also comes with an active PoE variant, so you can power the complete stack with a single RJ45 cable.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT/) - see the [LICENSE](LICENSE) file for details.

---

Made with &#x2764;&nbsp; by [**HARDWARIO s.r.o.**](https://www.hardwario.com/) in the heart of Europe.

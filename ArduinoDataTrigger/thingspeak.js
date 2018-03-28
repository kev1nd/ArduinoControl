var https = require('https');
var channel_id = process.env.ThingSpeakChannelId;     //ThingSpeak Channel ID
var speak_key = process.env.ThingSpeakKey;            //ThingSpeak Channel Read Key
var update_key = process.env.ThingSpeakUpdate;         //ThingSpeak Channel Update Key
var rsltval = 0;                     //Sensor Readings
var url = 'https://api.thingspeak.com/update?api_key=' + update_key;


exports.thingspeak = function (callback, content) {
    url += "&field1=" + content.therm;
    url += "&field2=" + content.temp;
    url += "&field3=" + content.humid;
    url += "&field4=" + content.light;
    url += "&field5=" + content.volts;
    https.get(url, function(res) {
        // Get latest sensor data from home IoT SoC
        res.on('data', function(d) {
            rsltval = d;
            callback(d);
        });
        res.on('end', function() {
        });
        res.on('error', function(e) {
            //context.fail("Got error: " + e.message);
        });
    });


};



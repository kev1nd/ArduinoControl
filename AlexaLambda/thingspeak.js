var https = require('https');
var channel_id = process.env.ThingSpeakChannelId;     //ThingSpeak Channel ID
var speak_key = process.env.ThingSpeakKey;            //ThingSpeak Channel Read Key
var p_light,p_tmp,p_hdt1,p_humid;                     //Sensor Readings
var url = 'https://api.thingspeak.com/channels/' + channel_id + '/feed/last.json?api_key=' + speak_key;

exports.thingspeak = function (callback) {

    https.get(url, function(res) {
        // Get latest sensor data from home IoT SoC
        res.on('data', function(d) {
            p_tmp = JSON.parse(d).field1;
            p_hdt1 = JSON.parse(d).field2;
            p_humid = JSON.parse(d).field3;
            p_light = JSON.parse(d).field4;
            callback(p_tmp,p_hdt1,p_humid,p_light);
        });
        res.on('end', function() {
        });
        res.on('error', function(e) {
            //context.fail("Got error: " + e.message);
        });
    });

};

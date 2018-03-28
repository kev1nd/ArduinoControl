//Environment Configuration

var config = {};
config.IOT_BROKER_ENDPOINT      = "a35en9jz3a4skz.iot.eu-west-1.amazonaws.com".toLowerCase();
config.IOT_BROKER_REGION        = "eu-west-1";
config.IOT_THING_NAME           = "mosquittobroker";

//Loading AWS SDK libraries
var AWS = require('aws-sdk');
AWS.config.region = config.IOT_BROKER_REGION;
//Initializing client for IoT
var iotData = new AWS.IotData({endpoint: config.IOT_BROKER_ENDPOINT});

/**
 * Activate the pump using AWS IoT Device Shadow
 */

exports.activateLed = function (intent, event, attributes, callback) {
    console.log("Led processing");
    var ledsetting = parseInt(event.request.intent.slots.Status.resolutions.resolutionsPerAuthority[0].values[0].value.id);
    
    var payloadObj = { "state":
                        { "desired":
                          {"ledstate":ledsetting}
                        }
                      };

    //Prepare the parameters of the update call
    var paramsUpdate = {
        //"thingName" : config.IOT_THING_NAME,
        "topic" : "$aws/things/mosquittobroker/shadow/update",
        "payload" : JSON.stringify(payloadObj)
    };

    console.log("Updating thing shaddow...");
    
    //Update Device Shadow updateThingShadow
    iotData.publish(paramsUpdate, function(err, data) {
      if (err){
        console.log("ERROR updating shaddow " + err);
      }
      else {
        console.log(data);
        callback(ledsetting);
      } 
    });
};



exports.setdelay = function (intent, event, attributes, callback) {
    console.log("Delay processing");
    var delaysetting = event.request.intent.slots.delay.value;
    console.log("Value received: " + delaysetting);
    
    var moment = require('moment');
    var x = moment.duration(delaysetting, moment.ISO_8601);
    var delaySeconds = x.asSeconds();
    var delayDesc = moment.duration(x, "minutes").humanize();
    
    console.log(delaySeconds);
    
    
    var delayMillis = 1000 * delaySeconds;
    var payloadObj = { "state":
                        { "desired":
                          {"delay":delayMillis}
                        }
                      };

    //Prepare the parameters of the update call
    var paramsUpdate = {
        "thingName" : config.IOT_THING_NAME,
        "payload" : JSON.stringify(payloadObj)
    };


    console.log("Updating thing shaddow...");
    
    //Update Device Shadow
    iotData.updateThingShadow(paramsUpdate, function(err, data) {
      if (err){
        console.log("ERROR updating shaddow " + err);
      }
      else {
        console.log(data);
        callback(delayDesc);
      } 
    });
};


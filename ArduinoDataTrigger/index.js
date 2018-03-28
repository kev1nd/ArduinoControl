const thingsp = require('./thingspeak');

exports.handler = (event, context, callback) => {
    // TODO implement
    //console.log("Trigger 3");
    //console.log(JSON.stringify(event));
    
    var myCallback = ((dataval) =>  {
        console.log('got data: '+ dataval);
    });
    thingsp.thingspeak(myCallback, event);
};


/* eslint-disable  func-names */
/* eslint quote-props: ["error", "consistent"]*/
/**
 * This sample demonstrates a sample skill built with Amazon Alexa Skills nodejs
 * skill development kit.
 * This sample supports multiple languages (en-US, en-GB, de-GB).
 * The Intent Schema, Custom Slot and Sample Utterances for this skill, as well
 * as testing instructions are located at https://github.com/alexa/skill-sample-nodejs-howto
 **/

'use strict';

const Alexa = require('alexa-sdk');
const thingsp = require('./thingspeak');
const iot = require('./iot');

const APP_ID = 'amzn1.ask.skill.051bebbf-4f0f-4f20-939f-42af736d2886'; 

const languageStrings = {
    'en': {
        translation: {
            SKILL_NAME: 'Arduino sensor reader',
            WELCOME_MESSAGE: "Welcome to %s. You can ask a question like, whats the current temperature? ... Now, what can I help you with?",
            WELCOME_REPROMPT: 'For instructions on what you can say, please say help me.',
            HELP_MESSAGE: "You can ask questions such as, what\'s the temperature, or, you can say exit...Now, what can I help you with?",
            HELP_REPROMPT: "You can say things like, what\'s the temperature, or you can say exit...Now, what can I help you with?",
            TEMP_MESSAGE: "The temperature reading is ",
            STOP_MESSAGE: 'Goodbye!',
            THERMO_MESSAGE: 'The thermistor reading is %f degrees. ',
            HD11_THERMO_MESSAGE: 'The HDT11 temperature reading is %i degrees. ',
            HD11_HUMIDITY_MESSAGE: 'The HDT11 humidity reading is %i percent. ',
            LIGHT_LEVEL_MESSAGE: 'The light level reading is %i. ',
            NO_SENSOR: 'Sorry, I didn\'t understand that sensor name.' ,
            LED_RESULT0: 'The LED is Off',
            LED_RESULT1: 'The LED is On',
            DELAY_RESULT: 'The delay time is set to %s'
        },
    },
    'en-US': {
        translation: {
            SKILL_NAME: 'American Arduino sensor reader',
        },
    },
    'en-GB': {
        translation: {
            SKILL_NAME: 'Arduino sensor reader',
        },
    },
};

const handlers = {
    'LaunchRequest': function () {
        this.attributes.speechOutput = this.t('WELCOME_MESSAGE', this.t('SKILL_NAME'));
        // If the user either does not reply to the welcome message or says something that is not
        // understood, they will be prompted again with this text.
        this.attributes.repromptSpeech = this.t('WELCOME_REPROMPT');
        this.emit(':ask', this.attributes.speechOutput, this.attributes.repromptSpeech);
    },
    'SwitchIntent': function () {
        console.log("SwitchIntent");
        var myCallback = ((p_led) =>  {
            console.log('LED status set to: ' + p_led);
            if (p_led == 0)
                this.attributes.speechOutput = this.t('LED_RESULT0');
            else
                this.attributes.speechOutput = this.t('LED_RESULT1');
            
            this.emit(':tell', this.attributes.speechOutput);
            });
        iot.activateLed('SwitchIntent', this.event, this.attributes, myCallback);
    },
    'DelayIntent': function () {
        console.log("SwitchIntent");
        var myCallback = ((p_delay) =>  {
            console.log('Delay between value posting set to: ' + p_delay);
             this.attributes.speechOutput = this.t('DELAY_RESULT', p_delay);
            
            this.emit(':tell', this.attributes.speechOutput);
            });
        iot.setdelay('DelayIntent', this.event, this.attributes, myCallback);
    },
    'TemperatureIntent': function () {
        var myCallback = ((p_tmp, p_hdt1, p_humid, p_light) =>  {
            console.log('got data: '+p_tmp);
            this.attributes.speechOutput = this.t('THERMO_MESSAGE', p_tmp);
            this.attributes.speechOutput += this.t('HD11_THERMO_MESSAGE', p_hdt1);
            this.emit(':tell', this.attributes.speechOutput);
            });
        thingsp.thingspeak(myCallback);
    },
    'LightLevelIntent': function () {
        var myCallback = ((p_tmp, p_hdt1, p_humid, p_light) =>  {
            console.log('got data: '+p_tmp);
            this.attributes.speechOutput = this.t('LIGHT_LEVEL_MESSAGE', p_light);
            this.emit(':tell', this.attributes.speechOutput);
            });
        thingsp.thingspeak(myCallback);
    },
    'SensorIntent': function () {
        var myCallback = ((p_tmp, p_hdt1, p_humid, p_light) =>  {
            const SensorSlot = this.event.request.intent.slots.SensorName;
            if (SensorSlot.resolutions && SensorSlot.resolutions.resolutionsPerAuthority && SensorSlot.resolutions.resolutionsPerAuthority[0].values) {
                let idref = parseInt(SensorSlot.resolutions.resolutionsPerAuthority[0].values[0].value.id);
                switch (idref) {
                    case 1: 
                        this.attributes.speechOutput = this.t('THERMO_MESSAGE', p_tmp);
                        break;
                    case 2: 
                        this.attributes.speechOutput = this.t('HD11_THERMO_MESSAGE', p_hdt1);
                        break;
                    case 3: 
                        this.attributes.speechOutput = this.t('HD11_HUMIDITY_MESSAGE', p_humid);
                        break;
                    case 4: 
                        this.attributes.speechOutput = this.t('LIGHT_LEVEL_MESSAGE', p_light);
                        break;
                    default:
                        this.attributes.speechOutput = "Sensor not found: " + SensorSlot.resolutions.resolutionsPerAuthority[0].values[0].value.id;
                }
            } else {
                //
                this.attributes.speechOutput = this.t('NO_SENSOR');
            }
            // this.attributes.speechOutput = this.t('THERMO_MESSAGE', p_tmp);
            // this.attributes.speechOutput += this.t('HD11_THERMO_MESSAGE', p_hdt1);
            // this.attributes.speechOutput += this.t('HD11_HUMIDITY_MESSAGE', p_humid);
            // this.attributes.speechOutput += this.t('LIGHT_LEVEL_MESSAGE', p_light);
            this.emit(':tell', this.attributes.speechOutput);
            });
        thingsp.thingspeak(myCallback);
    },
    'AMAZON.HelpIntent': function () {
        this.attributes.speechOutput = this.t('HELP_MESSAGE');
        this.attributes.repromptSpeech = this.t('HELP_REPROMPT');
        this.emit(':ask', this.attributes.speechOutput, this.attributes.repromptSpeech);
    },
    'AMAZON.RepeatIntent': function () {
        this.emit(':ask', this.attributes.speechOutput, this.attributes.repromptSpeech);
    },
    'AMAZON.StopIntent': function () {
        this.emit('SessionEndedRequest');
    },
    'AMAZON.CancelIntent': function () {
        this.emit('SessionEndedRequest');
    },
    'SessionEndedRequest': function () {
        this.emit(':tell', this.t('STOP_MESSAGE'));
    },
    'Unhandled': function () {
        this.attributes.speechOutput = this.t('HELP_MESSAGE');
        this.attributes.repromptSpeech = this.t('HELP_REPROMPT');
        this.emit(':ask', this.attributes.speechOutput, this.attributes.repromptSpeech);
    },
};

exports.handler = function (event, context) {
    const alexa = Alexa.handler(event, context);
    alexa.appId = APP_ID;
    // To enable string internationalization (i18n) features, set a resources object.
    alexa.resources = languageStrings;
    alexa.registerHandlers(handlers);
    alexa.execute();
};

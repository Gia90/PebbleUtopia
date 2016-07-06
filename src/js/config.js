module.exports = [
    { 
        "type": "heading", 
        "defaultValue": "Preferences" ,
        "size": 3
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Features"
            },
            {
              "type": "toggle",
              "messageKey": "SHOW_TEXT",
              "label": "Show text?",
              "defaultValue": false
            },
            {
              "type": "input",
              "messageKey": "CUSTOM_TEXT",
              "label": "Custom text",
              "attributes": {
                "placeholder": "Where is JESSICA HYDE?",
                "limit": 30,
                "type": "text"
              }
            }
        ]
    },
    { 
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Colors"
            },
            {
                "type": "select",
                "label": "Color Presets",
                "messageKey": "COLOR_PRESET",
                "defaultValue": 0,
                "options": [
                  { 
                    "label": "UTOPIA 1",
                    "value": 0
                  },
                  { 
                    "label": "UTOPIA 2",
                    "value": 1
                  },
                  { 
                    "label": "Custom",
                    "value": 2
                  }
                ]
            },
            {
              "type": "color",
              "messageKey": "BG_CUSTOM_COLOR",
              "label": "Background Color",
              "sunlight": false
            },
            {
              "type": "color",
              "messageKey": "TEXT_CUSTOM_COLOR",
              "label": "Text Color",
              "sunlight": false
            }
        ]
    },
    {
        "type": "submit",
        "defaultValue": "Save"
    },
    {
        "type": "heading",
        "defaultValue": "UTOPIA Watchface by Gia90",
        "size": 6
    }
];
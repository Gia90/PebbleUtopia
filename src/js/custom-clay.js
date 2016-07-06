// https://github.com/pebble/clay#custom-function
module.exports = function(minified) {
  var clayConfig = this;
  var $ = minified.$;

  // Enable option, only if 
  function disableEnable() {
    var text_is_on = clayConfig.getItemByMessageKey('SHOW_TEXT').get();
    
    console.log("TEXT is ", text_is_on);
    if(text_is_on)
    {
      console.log("TEXT ENABLE ");
      clayConfig.getItemByMessageKey('CUSTOM_TEXT').enable();
    }
    else
    {
      console.log("TEXT DISABLE");
      clayConfig.getItemByMessageKey('CUSTOM_TEXT').disable();
    }
    
    if (clayConfig.getItemByMessageKey('COLOR_PRESET').get() == "2")
    {
      clayConfig.getItemByMessageKey('BG_CUSTOM_COLOR').enable();
      if(text_is_on)
        clayConfig.getItemByMessageKey('TEXT_CUSTOM_COLOR').enable();
      else
        clayConfig.getItemByMessageKey('TEXT_CUSTOM_COLOR').disable();
    }
    else
    {
      clayConfig.getItemByMessageKey('BG_CUSTOM_COLOR').disable();
      clayConfig.getItemByMessageKey('TEXT_CUSTOM_COLOR').disable();
    }
  }
  
  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var toggle_show_text = clayConfig.getItemByMessageKey('SHOW_TEXT');
    var select_bg_color = clayConfig.getItemByMessageKey('COLOR_PRESET');
    disableEnable();
    select_bg_color.on('change', disableEnable);
    toggle_show_text.on('change', disableEnable);
  });
};
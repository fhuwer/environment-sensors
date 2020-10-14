#ifndef _PAGE_H_
#define _PAGE_H_

const String root_html = "<html>\
  <head>\
    <title>Environment Sensor Config</title>\
    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
    <style>\
      body { background-color: #ffffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\
      .col { float: left; box-sizing: border-box; padding: 0 .75rem; width: 100%; }\
      @media only screen and (min-width: 601px) { .col { width: 50%; } }\
      input:not([type=\"submit\"]) {\
        background-color: transparent;\
        border: none;\
        border-bottom: 1px solid #9e9e9e;\
        box-shadow: none;\
        height: 3rem;\
        width: 100%;\
        font-size: 16px;\
        margin: 0 0 8px 0;\
        padding: 0;\
        box-sizing: content-box;\
        transition: box-shadow .3s, border .3s;\
      }\
      input:valid, input:invalid { box-shadow: none; }\
      input:focus {\
        border-bottom: 1px solid #26a69a;\
        box-shadow: 0 1px 0 0 #26a69a;\
        outline: none;\
      }\
      .input-field > input:focus + label {\
        color: #26a69a;\
      }\
      .input-field > input:focus + label, .input-field > input:valid + label, .input-field > input[placeholder]:not([placeholder=\"\"]) + label {\
        transform: translateY(-14px) scale(0.8);\
        transform-origin: 0 0;\
      }\
      .input-field.col label { left: .75rem; }\
      .input-field > label {\
        color: #9e9e9e;\
        position: absolute;\
        top: 0;\
        left: 0;\
        cursor: text;\
        transition: transform .2s ease-out, color .2s ease-out;\
        transform-origin: 0% 100%;\
        transform: translateY(12px);\
      }\
      .input-field {\
        position: relative;\
        margin-top: 1rem;\
        margin-bottom: 1rem;\
      }\
      input[type=\"submit\"] {\
        text-decoration: none;\
        color: #fff;\
        background-color: #26a69a;\
        text-align: center;\
        letter-spacing: .5px;\
        border: none;\
        border-radius: 2px;\
        display: inline-block;\
        height: 36px;\
        line-height: 36px;\
        padding: 0 16px;\
        text-transform: uppercase;\
        vertical-align: middle;\
      }\
    </style>\
  </head>\
  <body>\
    <h1>Configuration</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/post/\">\
      <div class=\"row\">\
        <div class=\"col input-field\">\
          <input id=\"sensor_name\" name=\"sensor_name\" type=\"text\" pattern=\".+\" value=\"{{sensor_name}}\" required />\
          <label for=\"sensor_name\">Sensor name</label>\
        </div>\
        <div class=\"col input-field\">\
          <input id=\"sleep_time\" name=\"sleep_time\" type=\"text\" pattern=\".+\" value=\"{{sleep_time}}\" required />\
          <label for=\"sleep_time\">Sleep time (seconds)</label>\
        </div>\
      </div>\
      <div class=\"row\">\
        <div class=\"col input-field\">\
          <input id=\"wifi_ssid\" name=\"wifi_ssid\" type=\"text\" pattern=\".+\" value=\"{{wifi_ssid}}\" required />\
          <label for=\"wifi_ssid\">SSID</label>\
        </div>\
        <div class=\"col input-field\">\
          <input id=\"wifi_passphrase\" name=\"wifi_passphrase\" type=\"password\" pattern=\".+\" placeholder=\"{{wifi_passphrase_placeholder}}\" value=\"\" />\
          <label for=\"wifi_passphrase\">Passphrase (can not be empty currently)</label>\
        </div>\
      </div>\
      <div class=\"row\">\
        <div class=\"col input-field\">\
          <input id=\"network_ip\" name=\"network_ip\" type=\"text\" pattern=\".+\" value=\"{{network_ip}}\" required />\
          <label for=\"network_ip\">Device IP</label>\
        </div>\
        <div class=\"col input-field\">\
          <input id=\"network_gateway\" name=\"network_gateway\" type=\"text\" pattern=\".+\" value=\"{{network_gateway}}\" required />\
          <label for=\"network_gateway\">Gateway</label>\
        </div>\
      </div>\
      <div class=\"row\">\
        <div class=\"col input-field\">\
          <input id=\"network_dns\" name=\"network_dns\" type=\"text\" pattern=\".+\" value=\"{{network_dns}}\" required />\
          <label for=\"network_dns\">DNS</label>\
        </div>\
      </div>\
      <div class=\"row\">\
        <div class=\"col input-field\">\
          <input id=\"mqtt_hostname\" name=\"mqtt_hostname\" type=\"text\" pattern=\".+\" value=\"{{mqtt_hostname}}\" required />\
          <label for=\"mqtt_hostname\">MQTT Hostname / IP</label>\
        </div>\
        <div class=\"col input-field\">\
          <input id=\"mqtt_port\" name=\"mqtt_port\" type=\"text\" pattern=\".+\" value=\"{{mqtt_port}}\" required />\
          <label for=\"mqtt_port\">MQTT Port</label>\
        </div>\
      </div>\
      <div class=\"row\">\
        <div class=\"col input-field\">\
          <input id=\"mqtt_user\" name=\"mqtt_user\" type=\"text\" pattern=\".+\" value=\"{{mqtt_user}}\" required />\
          <label for=\"mqtt_user\">MQTT User</label>\
        </div>\
        <div class=\"col input-field\">\
          <input id=\"mqtt_password\" name=\"mqtt_password\" type=\"password\" pattern=\".+\" placeholder=\"{{mqtt_password_placeholder}}\" value=\"\" />\
          <label for=\"mqtt_password\">MQTT Password (can not be empty currently)</label>\
        </div>\
      </div>\
      <div class=\"row\" style=\"text-align: center;\">\
        <input type=\"submit\" name=\"save\" value=\"Save\" />\
        <input type=\"submit\" name=\"reboot\" value=\"Reboot\" />\
      </div>\
    </form>\
  </body>\
</html>";

#endif

/// Base ozj definition ############################################################

function $(id) {
    return document.getElementById(id);
}

function $_c(class_name) {
    return document.getElementsByClassName(class_name);
}

function getCssVar(name) {
    return getComputedStyle(document.documentElement).getPropertyValue(name);
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function ajaxGet(url, calback) {
    var ajax_request = new XMLHttpRequest();
    ajax_request.open('GET', url);
    ajax_request.onload = function () {
        var data = JSON.parse(ajax_request.responseText);
        calback(data);
    };
    ajax_request.send();
}

function ajaxSend(url, message, calback) {
    let headers = new Headers();

    headers.append('Content-Type', 'application/json');
    headers.append('Accept', 'application/json');


    fetch(url, {
        method: 'POST', // *GET, POST, PUT, DELETE, etc.
        body: message, // body data type must match "Content-Type" header
        headers: headers
    })
        .then(response => response.json())
        .then(data => {
            calback(data);
        })
        .catch((error) => {
            console.error('Error:', error);
        });
}


function mustacheRender(url, pageName, editData) {
    ajaxGet(url, function (data) {
        if (editData) editData(data);
        var template = $(pageName + "_template").innerHTML;
        var rendered = Mustache.render(template, data);
        $(pageName + "_container").innerHTML = rendered;
    });
}


// redefinition for post working
function ozConfirm(message) {
    return confirm(message);
}

// redefinition for post working
function ozAlert(message) {
    alert(message);
}





























// MENU SCTIPT ############################


function menu_hide_me() {
    $("menu_toggle").checked = false;
}

function menuSwitchPage(page_id) {
    menu_hide_me();

    var page = $(page_id);

    if (page.hidden) {

        var pages = $_c("main_article")
        for (var i = 0; i < pages.length; i++) {
            pages[i].hidden = true;
        }
        page.hidden = false;

        // on change page refresh content
        var refresh_func_name = "refresh_" + page_id;
        window[refresh_func_name]();
    }
}









































//  PAGES SECTION ########################################################

//var debug_url = "http://192.168.0.108";
//var debug_url = "http://192.168.1.122";
var debug_url = "";

var url_state = debug_url + "/state";
var url_log = debug_url + "/log";
var url_reboot = debug_url + "/reboot";
var url_unlock = debug_url + "/unlock";
var url_parameter = debug_url + "/parameter";
var url_save_parameter = debug_url + "/saveparameter";
var url_plugin_def = debug_url + "/plugindef";
var url_plugin = debug_url + "/plugin";
var url_save_plugin = debug_url + "/saveplugin";
var url_get_db = debug_url + "/getdb";
var url_save_db = debug_url + "/savedb";
var url_update = debug_url + "/update";
var url_clear_db = debug_url + "/cleardb";




document.addEventListener("DOMContentLoaded", function (event) {
    refresh_state_article();
});






















// SATE ###################################################################


var state_is_loaded = false;

function refresh_state_article() {
    $("header_page_title").innerHTML = "Stato";
    if (!state_is_loaded) {
        mustacheRender(url_state, "state", function (data) {
            if (data && data.config) {
                if (data.config.log && data.config.log == "n") {
                    $("log_menu_button").parentElement.style.display = "none";
                }
                if (data.config.ota && data.config.ota == "n") {
                    $("update_menu_button").parentElement.style.display = "none";
                }
            }
        });
        state_is_loaded = true;
    }
}



































// LOG ###################################################################

function refresh_log_article() {
    $("header_page_title").innerHTML = "Log";
    console.log("Refersh Log");
    mustacheRender(url_log, "log");
}





































// Parameter ###################################################################

var parameter_is_loaded = false;

function refresh_parameter_article() {
    $("header_page_title").innerHTML = "Parameters";
    if (!parameter_is_loaded) {
        mustacheRender(url_parameter, "parameter", function (data) {
            if (data)
                if (data.parameter)
                    data.parameter.reverse();
        });
        parameter_is_loaded = true;
    }
}

function parameterRestore(key) {
    console.log("Rest" + key);
    var paramValue = $("parameter_value_" + key);
    paramValue.value = $("parameter_hidden_" + key).value;
    paramValue.classList.remove("parameter_text_edited");
    $("parameter_restore_sub_" + key).style.display = getCssVar("--parameter-cell-display");
    $("parameter_restore_" + key).style.display = "none";
}

function parameterValueChange(key) {
    console.log("Change" + key);
    $("parameter_value_" + key).classList.add("parameter_text_edited");
    $("parameter_restore_sub_" + key).style.display = "none";
    $("parameter_restore_" + key).style.display = getCssVar("--parameter-cell-display");
}

function saveParameter() {
    var edited_paramenter = $_c("parameter_text_edited");
    var len = edited_paramenter.length;
    if (len > 0) {
        var confirm = ozConfirm("Save all " + len + " parameter?")
        if (confirm) {
            var send_data = "{";
            for (var i = 0; i < len; i++) {
                send_data += "\"" + edited_paramenter[i].getAttribute('data-name') + "\" : ";
                send_data += "\"" + edited_paramenter[i].value + "\"";
                if (i < len - 1)
                    send_data += ",";
            }
            send_data += "}";
            console.log(send_data);

            ajaxSend(url_save_parameter, send_data, function (data) {
                if (data.result == "O") {
                    ozAlert("Parameter Saved!!");
                    parameter_is_loaded = false;
                    refresh_parameter_article();
                }
                else {
                    ozAlert("An error occurred!!");
                }
            });
        }
    }
    else {
        ozAlert("No edited parameter to save");
    }
}












































// Plug-in ###################################################################

var plugin_is_loaded = false;
var plugin_setting_container = $("plugin_setting_container");
var plugin_setting_template = $("plugin_setting_template").innerHTML;

function refresh_plugin_article() {
    $("header_page_title").innerHTML = "Plugin";
    if (!plugin_is_loaded) {
        mustacheRender(url_plugin_def, "plugin_list");
        mustacheRender(url_plugin, "plugin_setting", function (data) {
            if (data && data.psetting) {
                for (var i = 0; i < data.psetting.length; i++) {
                    data.psetting[i].value = data.psetting[i].value.replace("{", "{\n").replaceAll(",", ",\n").replace("}", "\n}");
                }
            }
        });
        plugin_is_loaded = true;
    }
}

function addPlugin(key, caller) {
    var name = caller.innerHTML;
    var def = $("plugin_hidden_" + key).value.replace("{", "{\n").replaceAll(",", ",\n").replace("}", "\n}");

    const data = {
        psetting: [{
            name: name,
            key: key,
            value: def
        }]
    };

    var string = Mustache.render(plugin_setting_template, data);

    string.replace(",", ",\r\n");

    plugin_setting_container.innerHTML += string;
}

function removePlugin(item) {
    item.parentElement.remove();
}

function sendPlugin() {
    var confirm = ozConfirm("Save plugin ?")
    if (confirm) {
        var send_data = "[";
        var allPlugin = $_c("plugin_setting_text_area");
        var len = allPlugin.length;
        for (var i = 0; i < len; i++) {
            send_data += allPlugin[i].value;
            if (i < len - 1) {
                send_data += ",";
            }
        }
        send_data += "]";

        if (send_data.search("NaN") >= 0) {
            ozAlert("Field with [NaN] is required!! \n plese insert all required setting");
            return;
        }

        send_data.replace("'", "\"");

        ajaxSend(url_save_plugin, send_data, function (data) {
            if (data.result == "O") {
                ozAlert("Plugin Saved!!");
                plugin_is_loaded = false;
                refresh_plugin_article();
            }
            else {
                ozAlert("An error occurred!!");
            }
        });
    }
}










































// Settings ###################################################################

function refresh_setting_article() {
    $("header_page_title").innerHTML = "Settings";
}

function rebootESP() {
    if (ozConfirm("Device will reboot!! \n Are you sure?"))
        fetch(url_reboot, {
            method: 'GET', // *GET, POST, PUT, DELETE, etc.
            timeout: 1000
        }).then(response => response.json())
            .then(data => {
                if( data.result == "O")
                {
                    ozAlert("Device rebooting... \n Please wait... ");
                    sleep(1000).then(() => {
                        location.reload();
                    });
                }
                else
                {
                    ozAlert("An error occurrend");
                }
            })
            .catch((error) => {
                console.error('Error:', error);
            });
}

function unlockESP() {
    ajaxGet(url_unlock, function (data) {
        if (data && data.result && data.result == "O") {
            ozAlert("Device is unlocked!! \n Please reboot");
        }
        else {
            ozAlert("An server error occurred");
        }
    });
}

function removeDatabase() {
    if (ozConfirm("Clear all Database!! \n Are you sure to clear all data?"))
        if (ozConfirm("All data will erased!! \n Continue?"))
            ajaxGet(url_clear_db, function (data) {
                if (data && data.result && data.result == "O") {
                    ozAlert("All data is erased!! \n all parameter is set to dafult value \n please reboot to take effect");
                    location.reload();
                }
                else {
                    ozAlert("An server error occurred");
                }
            });
}

function downloadDatabase() {
    $("setting_download_link").click();
}


function uploadDatabase() {
    var file = $("upload_db_file").files[0];

    if (!file) {
        ozAlert('No File Selected!');
        return
    }

    var formdata = new FormData();
    formdata.append("upload_db_file", file);
    var ajax = new XMLHttpRequest();
    ajax.addEventListener("load", dbUpdateComplete, false);
    ajax.addEventListener("error", dbUpdateError, false);
    ajax.addEventListener("abort", dbUpdateErrorAbort, false);
    ajax.open("POST", url_save_db);
    ajax.send(formdata);
}

function dbUpdateComplete(event) {
    var result = event.target.responseText;
    if (result == "O") {
        ozAlert("Upload ok! \n please reboot to execute update!!");
        return;
    }
    ozAlert("Upload server error occurred");
}

function dbUpdateError(event) {
    ozAlert("Upload Error");
}

function dbUpdateErrorAbort(event) {
    ozAlert("Upload Aborted");
}
































// Ota ###################################################################

function refresh_update_article() {
    $("header_page_title").innerHTML = "OTA Update";
}


function uploadFile() {
    var file = $("upload_file").files[0];

    if (!file) {
        ozAlert('No File Selected!');
        return
    }

    var formdata = new FormData();
    formdata.append("upload_file", file);
    var ajax = new XMLHttpRequest();
    ajax.upload.addEventListener("progress", progressHandler, false);
    ajax.addEventListener("load", completeHandler, false);
    ajax.addEventListener("error", errorHandler, false);
    ajax.addEventListener("abort", abortHandler, false);
    ajax.open("POST", url_update);
    ajax.send(formdata);
}

function progressHandler(event) {
    $("upload_loaded_n_total").innerHTML = "Uploaded " + event.loaded + " bytes of " + event.total;
    var percent = (event.loaded / event.total) * 100;
    $("upload_progressBar").value = Math.round(percent);
    $("upload_status").innerHTML = Math.round(percent) + "% uploaded... please wait";
}

function completeHandler(event) {
    var result = event.target.responseText;
    if (result == "O") {
        ozAlert("Upload eseguito correttamente");
        $("upload_status").innerHTML = "Upload Ok"
    }

    if (result == "E") {
        $("upload_status").innerHTML = "Update ERROR ";
    }


    $("upload_progressBar").value = 0;
}

function errorHandler(event) {
    $("upload_status").innerHTML = "Upload Failed";
}

function abortHandler(event) {
    $("upload_status").innerHTML = "Upload Aborted";
}







































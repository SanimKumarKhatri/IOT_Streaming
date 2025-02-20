/*
 * index_script.c
 *
 *  Created on: Apr 17, 2020
 *      Author: User
 */

const char g_script_start[] = "<script>";

const char g_script_end[] = "</script>";

const char g_script_no_space[] =
"function nospaces(t){if(t.value.match(/\\s/g)){alert('This field cannot have spaces');"
"t.value=t.value.replace(/\\s/g,'');}}";

const char g_script_show_div[] =
"function showDiv(elem){if(elem.value == 0){document.getElementById('%s').style.display=\"block\";}else{"
"document.getElementById('%s').style.display=\"none\";}}";

const char g_script_progress_bar[] =
"function uploadFirmware() { const firmware = document.getElementById(\'firmware\').files[0];"
"if (!firmware) {alert(\"Please select a firmware file.\");return;}"
"document.getElementById('progress-container').style.display = 'block';"
"const formData = new FormData();formData.append(\'firmware\', firmware);"
"const xhr = new XMLHttpRequest(); xhr.open(\"POST\", \"/firmware\", true);"
"xhr.upload.onprogress = function(event) {if (event.lengthComputable) { const percentComplete = Math.round((event.loaded / event.total) * 100);"
"document.getElementById(\'progress-bar\').style.width = percentComplete + \'%\'; document.getElementById(\'progress-bar\').textContent = percentComplete + \'%\';"
"}};xhr.onload = function() { if (xhr.status === 200) {"
"document.getElementById(\'status-message\').textContent = \"OTA Update Successful! Device is rebooting...\";} else {"
"document.getElementById(\'status-message\').textContent = \"OTA Update Failed. Please try again.\"; }}; xhr.send(formData);}";

const char g_script_toggle[] =
"function toggle%s() {const textInput = document.getElementById(\"%s\");const checkbox = document.getElementById(\"%s\");"
"if (textInput && checkbox) {textInput.disabled = !checkbox.checked;console.log(\"Checkbox checked:\", checkbox.checked);console.log(\"Text input disabled:\", textInput.disabled);"
"} else {console.error(\"Elements not found. Make sure \'name\' attributes match in HTML.\");}}";

/*
 * index_html.c
 *
 *  Created on: Mar 29, 2020
 *      Author: User
 */

const char g_doc[] =
"<!DOCTYPE html><html style=\"background-color:#F0F0F0\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" charset=\"utf-8\">"
"<title>%s</title>";

const char g_header[] =
"<body><header><div id=\"header-inner\"><h2 class=\"name\">%s</h2><nav><a href=\"#\" id=\"menu-icon\">&#9776</a>"
"<ul><li><a href=\"/\" class=\"%s\">WiFi Settings</a></li><li><a href=\"mqtt_config\" class=\"%s\">MQTT Settings</a></li>"
"<li><a href=\"about\" class=\"%s\">About</a></li></ul></nav></div></header>";

const char g_container[] =
"<div class=\"container\">";

const char g_login_header[] =
"<body><header><div id=\"header-inner\"><h2 class=\"loginName\">%s</h2></header><div class=\"container\">";

const char g_form_start[] = "<form method=\"post\">";

const char g_ota_form_start[] = "<form method=\"post\" action=\"/firmware\" enctype=\"multipart/form-data\" id=\"upload_form\">";

const char g_row_str[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"text\" value=\"%s\" name=\"%s\" maxlength=\"%d\" %s %s></div></div>";

const char g_row_str1[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"text\" value=\"%s\" id=\"%s\" maxlength=\"%d\" %s %s></div></div>";

const char g_row_str2[] =
"<input type=\"text\" value=\"%s\" name=\"%s\" id =\"%s\" maxlength=\"%d\" %s %s></div></div>";

const char g_row_str_password[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"password\" value=\"%s\" name=\"%s\" maxlength=\"%d\" %s %s></div></div>";

const char g_row_str_ip[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"text\" value=\"%u.%u.%u.%u\" name=\"%s\""
"pattern=\"((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$\" %s></div></div>";

const char g_row_num[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"number\" value=\"%u\" name=\"%s\" min=\"%d\" required %s></div></div>";

const char g_row_file[] =
"<div class=\"center-container\"><input type=\"file\" name=\"firmware\" id=\"firmware\" accept=\".bin\">"
"<button class=\"btn\" onclick=\"uploadFirmware()\">Upload Firmware</div>";

const char g_progress_bar[] = 
"<div class=\"row\"><div id=\"progress-container\"><div id=\"progress-bar\">0%</div></div><div id=\"status-message\"></div></div>";

const char g_row_float[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"number\" value=\"%.2f\" name=\"%s\" %s></div></div>";

const char g_row_num1[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"number\" value=\"%u\" id=\"%s\" min=\"%d\" required %s></div></div>";

const char g_row_num2[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"number\" value=\"%d\" name=\"%s\" min=\"%d\" required %s></div></div>";

const char g_row_radio_select[] = 
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><input type=\"checkbox\" name=\"%s\" id=\"%s\" onclick=\"toggle%s()\" %s>";

const char g_row_select_list[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><select name=\"%s\" id=\"select\" %s>"
"<option value=\"0\" %s>%s</option><option value=\"1\" %s>%s</option><option value=\"2\" %s>%s</option></select></div></div>";

const char g_row_select_list2[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><select name=\"%s\" id=\"%s\" %s>"
"<option value=0 %s>%s</option><option value=1 %s>%s</option></select></div></div>";

const char g_row_select_list3[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><select name=\"%s\" id=\"%s\" %s>"
"<option value=0 %s>%s</option><option value=1 %s>%s</option><option value=2 %s>%s</option></select></div></div>";

const char g_row_select_list_hidden[] =
"<div class=\"row\"><div class=\"col-25\"><label>%s</label></div><div class=\"col-75\"><select name=\"%s\" id=\"select\" onchange=\"showDiv(this)\" %s>"
"<option value=\"1\"%s>%s</option><option value=\"0\"%s>%s</option></select></div></div>";

const char g_table_start[] = "<div><table>";

const char g_table_end[] = "</table></div>";

const char g_table_row[] = "<tr><th>%s</th><td>%s</td></tr>";

const char g_hide_div_start[] = "<div id=\"%s\" style=\"display:%s\">";

const char g_hide_div_end[] = "</div>";

const char g_ota_form_end[] = "<div class=\"row\"><input type=\"submit\" value=\"Update\"></div></form></div>";

const char g_form_end[] = "<div class=\"row\"><input type=\"submit\" value=\"Save\"></div></form></div>";

const char g_restart_form_end[] = "<div class=\"row\"><input type=\"submit\" value=\"RESTART\"></div></form></div>";

const char g_login_form_end[] = "<div class=\"row\"><input type=\"submit\" value=\"login\"></div></form></div>";

const char g_container_end[] = "</div>";

const char g_footer[] =
"<footer class=\"footer\">&#169 2024 Real Time Solutions Pvt. Ltd.</footer></body></html>";

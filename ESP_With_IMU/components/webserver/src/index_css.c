/*
 * index_css.c
 *
 *  Created on: Apr 17, 2020
 *      Author: User
 */

const char g_style_start[] = "<style>";

const char g_style_end[] = "</style></head>";

const char g_style[] =
"*{margin:0;padding:0;border:0;box-sizing:border-box;}html{height:100%;}body{position:relative;background:#f7f7f7;min-height:100%;color:#67727A;font-family:'Alegreya',sans-serif;margin:0;}input:-webkit-autofill{"
"-webkit-text-fill-color:#67727A !important;-webkit-box-shadow: 0 0 0 30px white inset;}.name{font-size:30px;font-weight:bold;float:left;display:block;padding:20px;}"
".loginName{font-size:30px;font-weight:bold;text-align:center;display:block;padding:25px;}"
"header{background-color:#f0f0f0;width:100%;height:86px;}#header-inner{margin:0 auto;}nav{float:right;padding: 20px 20px 0 0;}#menu-icon{position:absolute;"
"top:30px;right:30px;display:none;float:right;font-size:30px;color:#800040;text-decoration:none;}input[type=password],select,textarea{width:100%;"
"padding:12px;border: 1px solid #ccc;border-radius:4px;resize:vertical;color:#67727A;}#select{width:50%;}"
"a:hover#menu-icon{border-radius: 4px 4px 0 0;}ul{list-style-type:none;}nav ul li{font-family:'Alegreya Sans',sans-serif;";

const char g_style2[] =
"font-size:150%;display:inline-block;float:left;padding:5px;}nav ul li a{color:black;font-size:18px;padding:12px;"
"border-radius:36px;text-decoration:none;}nav ul li a:hover{background-color:#ddd;color:black;}nav ul li a.active{"
"background-color:#00b3b3;color:white;}input[type=text],select,textarea{width:100%;padding:10px;border:1px solid #ccc;border-radius:4px;resize:vertical;color:#67727A;}"
"input[type=number],select,textarea{width:100%;padding:12px;border:1px solid #ccc;border-radius:4px;resize:vertical;color:#67727A;}"
"label{padding: 12px 12px 12px 0;display:inline-block;font-weight:bold;}input[type=submit]{background-color:#4CAF50;color:white;padding: 12px 20px;"
"border:none;border-radius:4px;cursor:pointer;display:block;margin: 20px auto;}input[type=submit]:hover{background-color:#45a049;"
"}.footer{position:fixed;right:0;bottom:0;left:0;text-align:center;background-color:#f0f0f0;padding:10px 0 10px 0;}";

const char g_style3[] =
".container{border-radius:5px;background-color: #F7F7F7;padding: 40px 300px 40px 300px;}.col-25{float:left;width:25%;margin-top:6px;}"
".center-container {display: flex;flex-direction: column;align-items: center;justify-content: center;}"
".col-75{float:left;width:75%; margin-top:6px;display: flex;}.row{padding: 15px 0 15px 0;}.row:after{content:\"\";display:table;clear:both;}"
"@media screen and (max-width:1160px){header{position:inherit;height:100px;}.row{padding: 5px 0 5px 0;}.name{text-align:center;padding: 37px 80px 0 20px;font-size:25px;font-weight:bold;}"
"nav ul li{padding:15px;}.container{padding:50px 20px 50px 20px;}.col-25,.col-75{width:100%;margin-top:0;display:flex;}input[type=submit]{width:100%;margin-top:20px;}"
"#menu-icon{display:block;}#select{width:70%;}nav ul,nav:active ul{display:none;z-index:1000;position:absolute;padding:20px;"
"background:#F1F1F1;right:20px;top:60px;border:3px solid #FFF;border-radius:10px 0 2px 2px;width:70%;}nav:hover ul{display:block;}"
"nav li{text-align:left;width:100%;padding:1px 0;}nav ul li a{font-size:60%;}}";

const char g_style4[] =
"#progress-container { display: flex;justify-content: center;width: 20%;background-color: #ddd;text-align: center;border-radius: 5px; margin: auto;margin-top: 10px;text-align: center; display: none;}"
"#progress-bar { width: 0%; height: 20px; background-color: #4caf50; color: white;border-radius: 5px;}"
"#status-message{text-align: center; margin-top: 20px;font-weight: bold;}";

const char g_style5[] =
".btn{padding: 10px 20px; background-color: #4CAF50;color: white; border: none;border-radius: 5px;cursor: pointer;font-size: 1em;margin-top: 10px;width: 20%;}"
".btn:hover {background-color: #45a049;}";

const char g_style6[] =
"table {width: 80%;border-collapse: collapse;margin: 20px auto;}th, td {border: 1px solid #ccc;padding: 10px;text-align: left;}"
"th {background-color: #f4f4f4;font-weight: bold;}tr:nth-child(even){background-color: #f9f9f9;}tr:hover {background-color: #f1f1f1;}";
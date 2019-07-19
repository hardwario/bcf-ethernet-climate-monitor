#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

/*************************************************************************************
 * JavaScript Functions
 *************************************************************************************/

#define wiz550web_ajax_js 	"function AJAX(a,e){var c=d();c.onreadystatechange=b;function d(){if(window.XMLHttpRequest){return new XMLHttpRequest()}else{if(window.ActiveXObject){return new ActiveXObject(\"Microsoft.XMLHTTP\")}}}function b(){if(c.readyState==4){if(c.status==200){if(e){e(c.responseText)}}}}this.doGet=function(){c.open(\"GET\",a,true);c.send(null)};this.doPost=function(f){c.open(\"POST\",a,true);c.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\");c.setRequestHeader(\"ISAJAX\",\"yes\");c.send(f)}}function $(a){return document.getElementById(a)}function $$(a){return document.getElementsByName(a)}function $$_ie(a,c){if(!a){a=\"*\"}var b=document.getElementsByTagName(a);var e=[];for(var d=0;d<b.length;d++){att=b[d].getAttribute(\"name\");if(att==c){e.push(b[d])}}return e}"

/* Get: Network Information: function NetinfoCallback(o), getNetinfo() */
#define wiz550web_netinfo_js	"function NetinfoCallback(o){"\
									 "$('txtmac').value=o.mac;"\
									 "$('txtip').value=o.ip;"\
									 "$('txtgw').value=o.gw;"\
									 "$('txtsn').value=o.sn;"\
									 "$('txtdns').value=o.dns;"\
									 "if(typeof(window.external)!='undefined'){"\
										"obj=$$_ie('input','dhcp');"\
									 "}else{"\
										"obj=$$('dhcp');"\
									 "}"\
								"}"\
								" "\
								"function getNetinfo(){"\
									 "var oUpdate;"\
									 "setTimeout(function(){"\
									 	"oUpdate=new AJAX('get_netinfo.cgi',function(t){"\
									 		"try{eval(t);}catch(e){alert(e);}"\
									 "});"\
									 "oUpdate.doGet();},300);"\
								 "}"


/* Get: Digital I/O state/dir:  function DioCallback(o), getDio(o) */
/* Set: Digital I/O state: 		function setDiostate(o) */
/* Set: Digital I/O direction: 	function setDiodir(o) */
#define wiz550web_dio_js	"function DioCallback(o){"\
								"var pin = o.dio_p;"\
								"$('txtdio_s'+pin).value=o.dio_s;"\
								"$('txtdio_d'+pin).value=o.dio_d;"\
							"}"\
							"function getDio(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"oUpdate=new AJAX('get_dio'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();"\
							"}"\
							" "\
							"function setDiostate(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_s'+p).value;*/"\
								"var v=o.attributes['s'].value;"\
								"dout=new AJAX('set_diostate.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"\
							" "\
							"function setDiodir(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_d'+p).value;*/"\
								"var v=o.attributes['d'].value;"\
								"dout=new AJAX('set_diodir.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"


/* Get: Analog Input: 	function AinCallback(o), getAin(o) */
/* Additional function: function AinDrawgraph(o), getAin6_update() */
#define wiz550web_ain_js	"function AinCallback(o){"\
								"var pin = o.ain_p;"\
								"$('txtain_v'+pin).value=o.ain_v;"\
								"AinDrawgraph(o);"\
							"}"\
							"function getAin(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
							"}"\
							"function AinDrawgraph(o){"\
								"var pin=o.ain_p;"\
								"var val=o.ain_v;"\
								"$('ain_v'+pin).style.width=val*500/1023+'px';"\
							"}"\
							"function getAin6_update() {"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain6.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
								"setTimeout('getAin6_update()',500);"\
							"}"


/* Get: Analog Input: 	function AinCallback(o), getAin(o) */
/* Additional function: function AinDrawGoogleGauge(o), getAin6_update() */
#define ain_gauge_js		"google.load('visualization', '1', {packages:['gauge']});"\
							"google.setOnLoadCallback(AinDrawGoogleGauge);"\
							"function AinCallback(o){"\
								"var pin = o.ain_p;"\
								"$('txtain_v'+pin).value=o.ain_v;"\
								"AinDrawGoogleGauge(o);"\
							"}"\
							"function getAin(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
							"}"\
							"function AinDrawGoogleGauge(o){"\
								"var val=o.ain_v;"\
								"var temp_val=Number(((((val*3300)/1023)-500)/10).toFixed(2));"\
								"var data = google.visualization.arrayToDataTable(["\
									"['Label', 'Value'],"\
									"['Temp.', 80]"\
								"]);"\
								"var options = {"\
									"width:400, height:120,"\
									"max:100, min:-40,"\
									"greenFrom:-40, greenTo:0,"\
									"redFrom:90, redTo:100,"\
									"yellowFrom:75, yellowTo:90,"\
									"minorTicks: 5"\
								"};"\
								"var chart = new google.visualization.Gauge(document.getElementById('chart_div'));"\
								"data.setValue(0, 1, temp_val);"\
								"chart.draw(data, options);"\
							"}"\
							"function getAin6_update() {"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain6.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
								"setTimeout('getAin6_update()',500);"\
							"}"


/*************************************************************************************
 * HTML Pages (web pages)
 *************************************************************************************/

const char index_page[] =		"<html>"\
								"<head>"\
								"<title>BigClown Ethernet Module  Web Server</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"</head>"\
								"<body>"\
									"<div>"\
									"BigClown Ethernet Module Web Server Demo"\
									"</div>"\
									"<br>"\
									"<a href='netinfo.html'>Network Information</a>"\
									"<br>"\
									"<a href='img.html'>Base64 Image Data</a>"\
									"<br>"\
									"<br>"\
									"<a href='dio.html'>Ex1> Digital I/O</a>"\
									"<br>"\
									"<a href='ain.html'>Ex2> Analog Input</a>"\
									"<br>"\
									"<a href='ain_gauge.html'>Ex3> Analog Input: Google Gauge Chart</a>"\
								"</body>"\
							"</html>";


#define netinfo_page 		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>BigClown Ethernet Module Web Server Network Info</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<style>"\
								"label{float:left;text-align:left;width:50px;}"\
								"li {list-style:none;}"\
								"</style>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='netinfo.js'></script>"\
								"</head>"\
								"<body onload='getNetinfo();'>"\
									"<div>"\
									"BigClown Ethernet Module Web Server Network Information"\
									"</div>"\
									"<br>"\
									"<ul>"\
										"<li><label for='txtmac'>MAC:</label><input id='txtmac' name='mac' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtip'>IP:</label><input id='txtip' name='ip' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtgw'>GW:</label><input id='txtgw' name='gw' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtsn'>SN:</label><input id='txtsn' name='sn' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtdns'>DNS:</label><input id='txtdns' name='dns' type='text' size='20' disabled='disabled'/></li> "\
									"</ul>"\
								"</body>"\
							"</html>"


#define dio_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>BigClown Ethernet Module Web Server Digital I/O</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='dio.js'></script>"\
								"</head>"\
								"<body>"\
									"<!-- to do -->"\
									"<div>"\
									"<input type='button' value='LED R On' pin='8' s='1' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED R Off' pin='8' s= '0' onclick='setDiostate(this);'>"\
									"<br>"\
									"<input type='button' value='LED G On' pin='9' s='1' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED G Off' pin='9' s= '0' onclick='setDiostate(this);'>"\
									"<br>"\
									"<input type='button' value='LED B On' pin='10' s='1' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED B Off' pin='10' s= '0' onclick='setDiostate(this);'>"\
									"</div>"\
									"<!-- to do -->"\
								"</body>"\
							"</html>"


#define ain_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>BigClown Ethernet Module Web Server Analog Input</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
 	 	 	 	 	 	 	 	"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='ain.js'></script>"\
								"<style>"\
									".analog{margin-top:2px;margin-right:10px;border:1px solid #ccc;height:20px;width:500px;display:block;}"\
									".ain{width:0%;height:100%;text-align:center;background:red;float:left;display:block;}"\
								"</style>"\
								"</head>"\
								"<body>"\
									"<!-- to do -->"\
									"<div>"\
										"<input type='text' id='txtain_v6' size='5' disabled='disabled' value=''> "\
										"<input type='button' value='Get AIN' pin='6' onclick='getAin(this);'> "\
										"<input type='button' value='Get AIN Auto' onclick='getAin6_update();'> "\
										"<br>"\
										"<div class='analog' style='padding:0px;'><strong id='ain_v6' name='ain' class='ain' style='width:0%;'></strong></div>"\
									"</div>"\
									"<!-- to do -->"\
								"</body>"\
							"</html>"



#define ain_gauge_page		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>BigClown Ethernet Module Web Server Analog Input Gauge</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
 	 	 	 	 	 	 	 	"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='https://www.google.com/jsapi'></script>"\
								"<script type='text/javascript' src='ain_gauge.js'></script>"\
								"</head>"\
								"<body>"\
									"<div>"\
										"<input type='text' id='txtain_v6' size='5' disabled='disabled' value=''> "\
										"<input type='button' value='Get AIN' pin='6' onclick='getAin(this);'> "\
										"<input type='button' value='Get AIN Auto' onclick='getAin6_update();'> "\
										"<br>"\
										"<!--Draw the Google Gauge Chart-->"\
										"<div id='chart_div' style='width: 400px; height: 120px;'></div>"\
									"</div>"\
								"</body>"\
							"</html>"


/* Base64 Image converter: http://base64.wutils.com/encoding-online/ */
const char img_page[] =	"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>BigClown Ethernet Module Web Server Image</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
								"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"</head>"\
								"<body>"\
									"<div>"\
									"<img alt='wizwebserver.png'"\
									"src='data:image/png;base64,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'/>"\
									"</div>"\
								"</body>"\
							"</html>";

#endif

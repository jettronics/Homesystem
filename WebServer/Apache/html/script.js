
function restart_func () {
	document.getElementById("inforow").value = "Restarting Webserver ...";
}

function triggeralarm_func () {
	document.getElementById("inforow").value = "Trigger Test Alarm ...";
}

function getstatus_func () {
	document.getElementById("inforow").value = "Get Status ...";
}

window.onload = function () {
	document.getElementById("restart").onclick = restart_func;
	document.getElementById("triggeralarm").onclick = triggeralarm_func;
	document.getElementById("getstatus").onclick = getstatus_func;
};

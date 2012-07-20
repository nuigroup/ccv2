var host = '127.0.0.1';
var port = 7500;

var id = 0;

$.fn.serializeObject = function()
{
    var o = {};
    var a = this.serializeArray();
	var cur;
    $.each(a, function() {
		cur = (isNaN(this.value))?this.value:parseInt(this.value);
        if (o[this.name] !== undefined) {
            if (!o[this.name].push) {
                o[this.name] = [o[this.name]];
            }
            o[this.name].push(cur || 'error');
        } else {
            o[this.name] = cur || 'error';
        }
    });
    return o;
};

function syntaxHighlight(json) {
    if (typeof json != 'string') {
         json = JSON.stringify(json, undefined, 2);
    }
    json = json.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    return json.replace(/("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?)/g, function (match) {
        var cls = 'number';
        if (/^"/.test(match)) {
            if (/:$/.test(match)) {
                cls = 'key';
            } else {
                cls = 'string';
            }
        } else if (/true|false/.test(match)) {
            cls = 'boolean';
        } else if (/null/.test(match)) {
            cls = 'null';
        }
        return '<span class="' + cls + '">' + match + '</span>';
    });
}

function connect(host, port) {
	tcpClient = new TcpClient(host, port);
	tcpClient.connect(function() {
	  $('#status').attr("src", "grey.png");
	  tcpClient.addResponseListener(function(data) {
		$('#divDataRaw').empty();
		document.getElementById('divDataRaw').innerHTML = syntaxHighlight(data);
		$('#status').attr("src", "green.png");
		//$('#reload').attr("disabled", "disabled");
		});
	});
}

function updateJSON() {
	$('#divDataSent').empty();
	if(this.value == "null")
	{
		$('#divDataRaw').empty(); 
		return;
	}
	if(this.value == "/workflow/quit"){
		if(!confirm('Are you sure you want to quit?')) {return;}
	}
	var jsonRequest = {"jsonrpc": "2.0", 
		"method": document.getElementById("requests").value,
		"id" : id++,
		//"params" : JSON.parse(document.getElementById("params").value),
		"params" : $('form').serializeObject()
	};
	var stringreq = JSON.stringify(jsonRequest);
	$('#divDataSent').append(stringreq);
}


$(document).ready(function () {
	connect(host, port);

	$('#send').click(function() {
		tcpClient.sendMessage(document.getElementById("divDataSent").value, function() {$('#divDataRaw').empty(); $('#status').attr("src", "orange.png");});
		setTimeout(function() {if($('#divDataRaw').html() == "") { $('#status').attr("src", "red.png");$("#reload").removeAttr('disabled');}}, 1000);
	});
	$('#requests').change(updateJSON);
	var content = $('#requests').val();
    $('#params').keyup(function() { 
        if ($('#params').val() != content) {
            content = $('#params').val();
			updateJSON();
        }
    });

	$('.formy').change(updateJSON);
	$.each($('form').children(), function(index, value) {
		var orig = value.value;
		value.onkeyup = function() { 
			if (value.value != orig) {
				orig = value.value;
				updateJSON();
			}
		};
	});
	$('#reload').click(function(){connect(host, port);$("#reload").attr('disabled', 'disabled');})
});
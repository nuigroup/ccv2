var host = '127.0.0.1';
var port = 7500;

var id = 0;

var jsonReq = null;

var identifiers = ['pipeline', 'module', 'index', 'hosterName', 'sourceModule', 'sourcePort', 'destinationModule', 'destinationPort', 'author', 'description', 'identifier'];

var map = {'nui_list_pipelines' : 'hosterName', 'nui_create_pipeline' : ['pipeline'], 'nui_create_module' : ['pipeline', 'module'], 'nui_create_connection' : ['pipeline', 'sourceModule', 'sourcePort', 'destinationModule', 'destinationPort'],
'nui_update_pipeline' : ['pipeline', 'name', 'description', 'author'],'nui_update_module' : ['pipeline', 'index', 'description'],'nui_update_connection' : ['pipeline', 'sourceModule', 'sourcePort', 'destinationModule', 'destinationPort'],
'nui_delete_pipeline' : ['pipeline'],'nui_delete_module' : ['pipeline', 'module'],'nui_delete_connection' : ['pipeline', 'sourceModule', 'sourcePort', 'destinationModule', 'destinationPort'],
'nui_get_pipeline' : ['pipeline'],'nui_get_module' : ['pipeline', 'identifier'],'nui_get_connection' : ['pipeline', 'sourceModule', 'sourcePort', 'destinationModule', 'destinationPort'],'nui_navigate_push' : ['index'],'nui_navigate_pop' : []};

$.disjoin = function(a, b) {
    return $.grep(a, function($e) { return $.inArray($e, b) == -1; });
};

$.fn.serializeObject = function()
{
    var o = {};
    var a = this.serializeArray();
	var cur;
    $.each(a, function() {
		if(this.value != '') {
			cur = (isNaN(this.value))?this.value:parseInt(this.value);
			if (o[this.name] !== undefined) {
				//if ($('#'+this.name).attr('disabled') != 'disabled') {

					if (!o[this.name].push) {
						o[this.name] = [o[this.name]];
					}
					o[this.name].push((cur==0)?0:cur||null);
			} else {
				o[this.name] = (cur==0)?0:cur||null;
			}
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
		$('#divDataRaw').html('<pre>' + syntaxHighlight(JSON.stringify(JSON.parse(data), null, ' ')) + '</pre>');
		$('#status').attr("src", "green.png");
		});
	});
}

function updateRequest() {
	for(i in $.disjoin(identifiers, map[this.value])) {
		console.log($('#' + identifiers[i]).parent().parent());
		$('#' + identifiers[i]).parent().parent().addClass('disabled');
		$('#' + identifiers[i]).attr('disabled', 'disabled');
	}
	for(i in map[this.value]) {
		$('#' + map[this.value][i]).parent().parent().removeClass('disabled');
		$('#' + map[this.value][i]).removeAttr('disabled');
	}
	updateJSON();
}

function updateJSON() {
	$('#divDataSent').empty();
	if(this.value == "null")
	{
		$('#divDataRaw').empty(); 
		return;
	}
	if(this.value == "workflow_quit"){
		if(!confirm('Are you sure you want to quit?')) {return;}
	}
	var jsonRequest = {"jsonrpc": "2.0", 
		"method": $('#requests').val(),
		"id" : id,
		"params" : $('form').serializeObject()
	};
	var stringreq = '<pre>' + syntaxHighlight(JSON.stringify(jsonRequest, null, ' ')) + '</pre>';
	$('#divDataSent').html(stringreq);
	jsonReq = JSON.stringify(jsonRequest);
}


$(document).ready(function () {
	connect(host, port);

	$('#send').click(function() {
		tcpClient.sendMessage(jsonReq, function() {$('#divDataRaw').empty(); $('#status').attr("src", "orange.png");});
		setTimeout(function() {if($('#divDataRaw').html() == "") { $('#status').attr("src", "red.png");$("#reload").removeAttr('disabled');}}, 1000);
		id++;
		updateJSON();
	});
	$('#requests').change(updateRequest);

	var input = $('input');
	$.each($('form').find(input), function(index, value) {
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
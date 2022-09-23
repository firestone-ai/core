(function (window, undefined) {

	window.hyphen.loadModule();

	var textarea = document.getElementById("textarea");
	var form = document.querySelector("form");
	var combobox = document.getElementById("combobox");

	var application = undefined;
	
	form.onsubmit = function(event) {

		if(combobox.value == "") {
			return;
		}

		if(application == undefined) {
			application = window.hyphen.hyphenCreateApplication();
		}

		var lang = combobox.value;
		var text = textarea.value.split("\n").join(" ").split(" ");
	 
		var request = new XMLHttpRequest();
		var path = '../../../../../../dictionaries/' + lang + '/' + 'hyph_' + lang  + '.dic';
		request.open('GET', path, true);
		request.send(null);

		request.onload = function () {
			var dict = request.responseText;
			window.hyphen.hyphenLoadDictionary(application, dict, lang);

			for(var i = 0; i < text.length; i++) {
				var hyphens = window.hyphen.hyphenWord(application, text[i].toLowerCase(), lang);
				console.log(hyphens);
			}
		}
	   event.preventDefault();
	}
})(self);
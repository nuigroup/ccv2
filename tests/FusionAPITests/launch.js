chrome.experimental.app.onLaunched.addListener(function() {
  chrome.appWindow.create('tests.html', {
    width: 680,
    height: 480
  });
});

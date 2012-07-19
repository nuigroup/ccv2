chrome.experimental.app.onLaunched.addListener(function() {
  chrome.appWindow.create('tests.html', {
    width: 800,
    height: 600
  });
});

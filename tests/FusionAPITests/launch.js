chrome.experimental.app.onLaunched.addListener(function() {
  chrome.app.window.create('tests.html', {
    width: 800,
    height: 600
  });
});

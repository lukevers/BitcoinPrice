Pebble.addEventListener('ready', function(e) {
    updatePrice();
});

function updatePrice() {
    var req = new XMLHttpRequest();
    req.open('GET', 'https://api.bitcoinaverage.com/all');
    req.onload = function(e) {
        console.log('Received response!');
        var prices = JSON.parse(req.responseText);
        var last = prices.USD['averages']['last'];
        console.log('Last price: ' + last);
        Pebble.sendAppMessage({'BtcPrice': JSON.stringify(last) });
    }
    req.send(null);
}

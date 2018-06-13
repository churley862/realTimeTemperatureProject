var host = window.location.host;
var data = [];
var g = undefined;

var getData = function() {
  jQuery.getJSON(`http://${host}/temp`, function(result) {
    data = result.map(function(row) { row[0] = new Date(row[0] * 1000); return row; });
    displayGraph();
  });
};

var getLabels = function() {
  var maxDevice = 0;
  data.forEach(function(elem) {
    maxDevice = Math.max(maxDevice, elem.length);
  });

  var result = ["TimeStamp"];
  for (var i = 1; i < maxDevice; ++i) {
    result.push("Device" + i);
  }

  return result;
};

var displayGraph = function()  {
  if (typeof(g) === "undefined") {
    g = new Dygraph(document.getElementById("graph1"), data, {
      labels: getLabels(),
      drawCallback: function(g, is_initial) {
        if (is_initial) {
          $("#graph1").css("background-image", "none");
          $("#graph1").css("background", "#fefefe");
        }
      }
    });

  } else {
    g.updateOptions({ file: data });
  }
};


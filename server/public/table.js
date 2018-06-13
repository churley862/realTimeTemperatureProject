var host = window.location.host;
var data = [];

var getData = function() {
  jQuery.getJSON(`http://${host}/temp`, function(result) {
    data = result.map(function(row) { row[0] = new Date(row[0] * 1000); return row; });
    return data;
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
chart = new Highcharts.Chart({
    chart: {
      type: 'column',
      renderTo: 'Collected-data-chart'
    },
    title: {
      text: 'Collected Temperature data'
    },
    yAxis: {
      min: 0,
      title: {
        text: 'Temperature (degrees fairenhight)'
      }
    },
    series: [{
      name: 'Temperature',
      color: '#006A72'
    }]
  });

function updateChart() {
   var dynatable = $table.data('dynatable'), categories = [], values = [];
   $.each(dynatable.settings.dataset.records, function() {
     categories.push(getLabels());
     values.push(getData());
     dynatable.display();
   });
    chart.xAxis[0].setCategories(categories);
    chart.series[0].setData(values);
  };

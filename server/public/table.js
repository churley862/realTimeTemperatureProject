var host = window.location.host;
var data = [];
var columns = {};

var getData = function() {
  jQuery.getJSON(`http://${host}/temp`, function(result) {
    new_columns = []
    result = result.map(function(row) { 
      o = {}
      o["time"] = (new Date(row[0] * 1000)); 
      for(var i = 1; i < row.length; ++i) {
        if (row[i] != null) {
          var colName = `Device${i}`;
          columns[colName] = true;
          o[colName.toLowerCase()] = row[i];
        }
      }
      return o; 
    });

    data = result;
  });
};

var buildTable = function() {
  $('#table1').empty();
  $('#table1').append("<thead>");
  $('#table1 thead').append("<tr>");
  $('#table1 thead tr').append("<th>Time</th>");
  for (var cname in columns) {
    $('#table1 thead tr').append(`<th>${cname}</th>`);
  }
  $('#table1').append("<tbody>");

  $('#table1').dynatable({
    dataset: {
      records: data
    }
  })
}


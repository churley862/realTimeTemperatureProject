# myapp.rb

at_exit do
  puts "Cleanup sinatra"
end

require 'sinatra'
require 'json'

# dev, time, temp

data = []
sorted = {}

get '/' do
  redirect '/index.html'
end

post '/temp/:id/:timestamp/:value' do
  elem = [ params[:id].to_i, params[:timestamp].to_i, params[:value].to_f ]
  data << elem
  sorted[elem[1]] ||= []
  sorted[elem[1]][elem[0]] = elem[2]
  data.length.to_s
end

get '/devices' do
  data.map { |id, time, temp| id }.uniq.to_json
end

get '/time' do 
  Time.now.to_i.to_s
end

get '/temp' do #  [?begin=timestamp&end=timestamp]
  #sorted = {}
  #data.each do |id, time, temp| 
    #sorted[time] ||= []
    #sorted[time][id] = temp
  #end
  result = []
  sorted.keys.sort.each do |at|
    result << [at, *sorted[at]]
  end 
  result.to_json
end

get '/temp/:id' do  # [?begin=timestamp&end=timestamp]
  data.select { |id, x, y| id == params[:id].to_i }.to_json
end

get '/config/:id' do 
end

get '/status' do 
  result = {}
  data.each do |id, time, temp|
    result[id] = { id: id, time: time, temp: temp }
  end
  result.to_json
end

post '/config/:id' do # { json config file POST data }
end

post '/time/:time' do 
  proposed = Time.at(params[:time].to_i)
  actual = Time.now
  if ((proposed-actual).abs > 100)
    current = proposed.strftime("%m%d%H%M%Y.%S")
    system("sudo date -u #{current}")
    "Time set"
  else
    "Too soon"
  end
end

post '/reset' do
  data = []
end




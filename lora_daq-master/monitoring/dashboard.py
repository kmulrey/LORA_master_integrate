# Dashboard for monitoring LORA DAQ


import dash
import datetime
import base64
import dash_core_components as dcc
import dash_html_components as html
import numpy as np
import glob
from calendar import timegm
from  matplotlib import cm
from dash.dependencies import Input, Output
import plotly.graph_objs as go
import sys
from time import gmtime
from dash.exceptions import PreventUpdate
mycmap=[]
mycmap.append("rgb(%i,%i,%i)"%(255,255,255))
for i in range(1,256,32):
    for j in range(4):
        mycmap.append("rgb(%i,%i,%i)"%(255*cm.coolwarm(i)[0],255*cm.coolwarm(i)[1],255*cm.coolwarm(i)[2]))

glob_sta_info_current={}
glob_arr_info_current={}
glob_drop_down_value=sorted(glob.glob("logfiles/20*_array.log")+glob.glob("logfiles/*/20*_array.log"))[-1].split("_array")[0]
glob_remember_det_selection=1
glob_radio_button_val=30e3

def strip_time(str):
    return str.split(' ')[0]

def strip_date(str):
    return str.split(' ')[1]

def get_file_list():
    k=sorted(glob.glob("logfiles/20*array.log")+glob.glob("logfiles/*/20*array.log") )[-50:]
    k=k[::-1]
    j=[m.split('_array')[0] for m in k]
    dictlist=[]
    for i in j:
        dictlist.append({'label':str(i),'value':str(i)})
    return dictlist

glob_file_dictlist=get_file_list()

def convert_to_time_str(t):
    t1=gmtime(t)
    year=t1.tm_year
    mon=t1.tm_mon
    day=t1.tm_mday
    hr = t1.tm_hour
    min = t1.tm_min
    sec = t1.tm_sec
    val = "%.4i-%.2i-%.2i %.2i:%.2i:%.2i"%(year,mon,day,hr,min,sec)
    return val

def load_sta_info_file(tfile):
    print (tfile)
    temp=np.loadtxt(tfile).T
    GPS_time_stamp, det_no, baseline, sigma, n_trig, thresh = temp

    dict2={
    'time':[],
    'id':[],
    'baseline':[],
    'sigma':[],
    'n_trigs':[],
    'thresh':[]
    }

    dict2['time'].extend(GPS_time_stamp)
    dict2['id'].extend(det_no)
    dict2['baseline'].extend(baseline)
    dict2['sigma'].extend(sigma)
    dict2['n_trigs'].extend(n_trig)
    dict2['thresh'].extend(thresh)

    return dict2

def load_array_info_file(tfile):
    print(tfile)
    temp=np.loadtxt(tfile).T
    GPS_time_stamp = temp[0]
    nsec = temp[1]
    ndet, nsta, tot_charge, core_x, core_y = temp[2:7]
    lora_trig, lofar_trig, trigg_sent = temp[7:10]
    charges=temp[10:10+20]
    peaks=temp[10+20:]

    dict2={
    'time':[],
    'nsec':[],
    'ndet':[],
    'nsta':[],
    'lora_trig':[],
    'lofar_trig':[],
    'trigg_sent':[],
    'tot_charge':[],
    'core_x':[],
    'core_y':[],
    'charges':[],
    'charges_detwise':[],
    'peaks_detwise':[]
    }

    dict2['time'].extend(GPS_time_stamp)
    dict2['trigg_sent'].extend(trigg_sent)
    dict2['nsec'].extend(nsec)
    dict2['ndet'].extend(ndet)
    dict2['nsta'].extend(nsta)
    dict2['lora_trig'].extend(lora_trig)
    dict2['lofar_trig'].extend(lofar_trig)
    dict2['tot_charge'].extend(tot_charge)
    dict2['core_x'].extend(core_x)
    dict2['core_y'].extend(core_y)
    for i in charges.T:
        dict2['charges'].append(i)#/this is event wise
    for i in charges:
        dict2['charges_detwise'].append(i)
    for i in peaks:
        dict2['peaks_detwise'].append(i)

    return dict2

def merge_dicts(dict1,dict2):
    assert(dict1.keys()==dict2.keys())
    for key in dict1.keys():
        dict1[key].extend(dict2[key])
    return dict1

def load_station_file_current():
    global glob_drop_down_value,glob_sta_info_current
    glob_sta_info_current=load_sta_info_file(glob_drop_down_value+"_detectors.log")
    return

def load_array_file_current():
    global glob_drop_down_value,glob_arr_info_current
    glob_arr_info_current=load_array_info_file(glob_drop_down_value+"_array.log")
    return

def return_radio_trig_text(xdata,key):
    a= np.array(xdata['lofar_trig'])
    d = np.array([convert_to_time_str(i) for i in xdata['time']])

    select=a==1
    d=d[a==1]


    if len(d)==0:
        return ''
    else:
        if key=='last':
            text="Last Radio Trigger @ "
            return text+d[-1]
        elif key=='list':
            text="List of Radio Triggers: "
            text2=', '.join(d)
            return text+text2
        else:
            return ''

def return_time_figure(det,xdata):
    a= np.array(xdata['id'])
    b= np.array(xdata['n_trigs'])
    c= np.array(xdata['time'])
    d = np.array([convert_to_time_str(i) for i in xdata['time']])

    e= np.array(xdata['baseline'])
    f= np.array(xdata['sigma'])
    g= np.array(xdata['thresh'])

    select = a==det
    traces=[]
    x_ticks=[]
    x_ticklabels=[]
    if len(select[select])!=0:
        traces.append(go.Scatter(x=c[select], y=b[select], text=d[select], mode='markers',name='N Trigs/past 5 mins'))
        traces.append(go.Scatter(x=c[select], y=e[select], text=d[select], mode='markers',name='Avg Baseline/past 5 mins'))#,yaxis='y2'))
        traces.append(go.Scatter(x=c[select], y=f[select], text=d[select], mode='markers',name='Avg Sigma/past 5 mins'))#,yaxis='y3'))
        traces.append(go.Scatter(x=c[select], y=g[select], text=d[select], mode='markers',name='Threshold@GivenTime'))#,yaxis='y3'))

        x_ticks=np.linspace(np.amin(c[select]), np.amax(c[select]),20,dtype=np.int)
        x_ticklabels=[]
        prev_date = strip_time(convert_to_time_str(x_ticks[0]))
        x_ticklabels.append(prev_date)
        for i in range(1,len(x_ticks)):
            t_str=convert_to_time_str(x_ticks[i])
            date_new = strip_time(t_str)
            if date_new!=prev_date:
                x_ticklabels.append(date_new)
            else:
                x_ticklabels.append(strip_date(t_str))
            prev_date = date_new

    return {
        'data':traces,
        'layout': {
            'title': 'Det %s'%det,
            'titlefont':{'size':26},
            'xaxis':{'title':'Time',
                     'titlefont':{'size':24},
                     'tickfont':{'size':14},
                     'tickmode':'array',
                     'tickvals':x_ticks,
                     'ticktext':x_ticklabels
                     },
            'yaxis':{'title':'',
                     'titlefont':{'size':24},
                     'tickfont':{'size':14}},
        }
    }

def return_time_figure_2(xdata):
    b= np.array(xdata['ndet'])
    e= np.array(xdata['nsta'])
    c= np.array(xdata['time'])
    d = np.array([convert_to_time_str(i) for i in xdata['time']])
    hover_txt=np.array([d[i] + "/ logQ:%.3f"%np.log10(xdata['tot_charge'][i]) for i in range(len(d))])


    traces=[]
    traces.append(go.Scatter(x=c, y=b, text=hover_txt, mode='markers',name='N Dets'))
    traces.append(go.Scatter(x=c, y=e, text=hover_txt, mode='markers',name='N Statns'))

    x_ticks=np.linspace(np.amin(c), np.amax(c),20,dtype=np.int)
    x_ticklabels=[]
    prev_date = strip_time(convert_to_time_str(x_ticks[0]))
    x_ticklabels.append(prev_date)
    for i in range(1,len(x_ticks)):
        t_str=convert_to_time_str(x_ticks[i])
        date_new = strip_time(t_str)
        if date_new!=prev_date:
            x_ticklabels.append(date_new)
        else:
            x_ticklabels.append(strip_date(t_str))
        prev_date = date_new

    return {
        'data':traces,
        'layout': {
            'hovermode': 'closest',
            'title': '',
            'titlefont':{'size':26},
            'xaxis':{'title':'Time',
                     'titlefont':{'size':24},
                     'tickfont':{'size':14},
                     'tickmode':'array',
                     'tickvals':x_ticks,
                     'ticktext':x_ticklabels
                     },
            'yaxis':{'title':'','font-size':'xxlarge',
                     'titlefont':{'size':24},
                     'tickfont':{'size':14}},
                     'dtick':1,
        }
    }

def return_det_id_figure(d,key,cmap,title):
    b= np.array(d[key])
    a= np.array(d['id'])
    c= np.array(d['time'])
    d = np.array([convert_to_time_str(i) for i in d['time']])

    c,a,b,d=np.array([[x1,x2,x3,x4] for x1,x2,x3,x4 in sorted(zip(c,a,b,d))]).T

    list_of_times=np.unique(c)

    cmap = cm.get_cmap(cmap)

    traces=[]#list of dictionaries
    n=6
    counter=0
    for time in list_of_times[-1*n*5::][::5]:
        counter+=1
        select = c==time
        temp={}
        temp['x']= a[select]
        temp['y']= b[select]
        temp['type']='Scatter'
        temp['name']=d[select][0]
        temp['mode']='markers'
        #temp['colorscale']='Viridis'
        temp['marker']={'color':"rgb(%s,%s,%s)"%cmap(1.0*counter/n)[:3],
                        'size':12}
        #https://plot.ly/python/v3/figure-labels/#dash-example
        traces.append(temp)

    return {
        'data': traces,
        'layout': {
            'title': title,
            'titlefont':{'size':26},
            'xaxis':{'title':'Detector Id',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     'dtick':1,
                     'tickfont':{'size':14}},
            'yaxis':{'title':'','font-size':'xxlarge',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     'tickfont':{'size':14}},
        }
    }

def return_run_start_time(xdata):
    d = np.array([convert_to_time_str(i) for i in xdata['time']])
    if len(d)==0:
        d=['']
    return "Run Start Time: "+d[0] +" ."

def get_last_load_time():
    a=str(datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S"))
    b=" Updated UTC: "
    return b+a+" ."

def return_evt_display_histogram(xdata):
    global glob_det_x, glob_det_y,glob_id
    x=np.array(glob_det_x)
    xrange=[-240,240]
    xbinsize=5

    y=np.array(glob_det_y)
    yrange=[-240,240]
    ybinsize=5

    glob_id=np.array(glob_id)
    z=np.array(xdata['charges'][-1])

    traces=[]
    traces.append(
        go.Scatter(x=glob_det_x[z!=0], y=glob_det_y[z!=0],text=glob_id[z!=0],
                   mode='markers',
                   marker={'color':np.log10(z[z!=0]),
                            'colorscale':mycmap[1:], 'size':10,
                            'colorbar':{'thickness':15,'title':'log charge'}
                          }
                    )
        )
    traces.append(
        go.Scatter(x=glob_det_x, y=glob_det_y,#text=glob_id,
                   mode='markers',
                   marker={'color':'rgba(0, 0, 0, 0.0)',#transparent circle.
                           'line':{'width':1,'color':'black'},'size':8,
                           })
        )


    return {
        'data': traces,
        'layout': {
            'showlegend':False,
            'hovermode': 'closest',
            'title': 'Most Recent Event',
            'titlefont':{'size':26},
            'xaxis':{'title':'X (m)',
                     'titlefont':{'size':24},
                     'zeroline':False,
                     'dtick':80,
                     'tickfont':{'size':12},
                     'showgrid':False,
                     'range':[xrange[0], xrange[1]]},
            'yaxis':{'title':'Y (m)',
                     'titlefont':{'size':24},
                     'zeroline':False,
                     'dtick':80,
                     'tickfont':{'size':12},
                     'showgrid':False,
                     'range':[yrange[0], yrange[1]]},
        }
        }

def return_core_histogram(xdata):
    global glob_det_x, glob_det_y,glob_id
    x=xdata['core_x']
    xrange=[-240,240]
    xbinsize=10

    y=xdata['core_y']
    yrange=[-240,240]
    ybinsize=10

    traces=[]
    traces.append(
        go.Histogram2d(x=x, y=y,
        autobinx=False,
        xbins=dict(start=xrange[0], end=xrange[1], size=xbinsize),
        autobiny=False,
        ybins=dict(start=yrange[0], end=yrange[1], size=ybinsize),
        colorscale=mycmap,
        colorbar = {'thickness':15,'title':'counts'},
        zmin=0,
        ))
    traces.append(
        go.Scatter(x=glob_det_x, y=glob_det_y,text=glob_id,
                   mode='markers',name='Detectors',
                   marker={'color':'rgba(0, 0, 0, 0.0)',#transparent circle.
                           'line':{'width':1,'color':'black'},'size':8,
                           })
        )

    return {
        'data': traces,
        'layout': {
            'hovermode': 'closest',
            'title': 'Approx. Core Histogram',
            'titlefont':{'size':26},
            'xaxis':{'title':'X (m)',
                     'titlefont':{'size':24},
                     'zeroline':False,
                     'dtick':80,
                     'showgrid':False,
                     'tickfont':{'size':12}},
            'yaxis':{'title':'Y (m)',
                     'titlefont':{'size':24},
                     'zeroline':False,
                     'showgrid':False,
                     'dtick':80,
                     'tickfont':{'size':12}},
        }
        }

def return_charge_spectrum(xdata):
    q=np.log10(xdata['tot_charge'])
    # qrange=[-200,200]
    qbinsize=0.05

    traces=[]
    traces.append(
        go.Histogram( x=q, xbins=dict(size=qbinsize) )
        )

    return {
        'data': traces,
        'layout': {
            'title': 'Charge Spectrum',
            'titlefont':{'size':26},
            'xaxis':{'title':'log10(Total Charge (ADC))',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     #'dtick':40,
                     'tickfont':{'size':14}},
            'yaxis':{'title':'N',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     #'dtick':40,
                     'tickfont':{'size':14}},
        }
        }

def return_charge_spectrum_det(xdata,det):
    q=xdata['charges_detwise'][det-1]
    q=q
    qbinsize=2.5
    xrangef=np.amax([int(np.median(q)),1400])

    traces=[]
    if not (q==0).all():
        traces.append(
            go.Histogram( x=q, xbins=dict(size=qbinsize) )
            )

    return {
        'data': traces,
        'layout': {
            'title': 'Det %s Charge Spectrum'%det,
            'titlefont':{'size':26},
            'xaxis':{'title':'Charge (ADC)',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     #'dtick':40,
                     'range':[-150,xrangef],
                     'tickfont':{'size':14}},
            'yaxis':{'title':'N',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     'type':'log',
                     #'dtick':40,
                     'tickfont':{'size':14}},
        }
        }

def return_peak_spectrum_det(xdata,det):
    q=xdata['peaks_detwise'][det-1]
    q=q
    qbinsize=2
    xrangef=np.amax([int(np.median(q)),200])

    traces=[]
    if not (q==0).all():
        traces.append(
            go.Histogram( x=q, xbins=dict(size=qbinsize) )
            )

    return {
        'data': traces,
        'layout': {
            'title': 'Det %s Peak Spectrum'%det,
            'titlefont':{'size':26},
            'xaxis':{'title':'Charge (ADC)',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     #'dtick':40,
                     'range':[-20,xrangef],
                     'tickfont':{'size':14}},
            'yaxis':{'title':'N',
                     'titlefont':{'size':24},
                     'zeroline':True,
                     'type':'log',
                     #'dtick':40,
                     'tickfont':{'size':14}},
        }
        }

#initiate data.
load_station_file_current()
load_array_file_current()
glob_id, glob_det_x, glob_det_y, temp=np.loadtxt('../input/detector_coord.txt').T
glob_id=[str(id) for id in glob_id]

app = dash.Dash()

vub_logo = 'vub_logo.jpg' # replace with your own image
encoded_vub_logo = base64.b64encode(open(vub_logo, 'rb').read())

ru_logo = 'ru_logo.jpg'
encoded_ru_logo = base64.b64encode(open(ru_logo, 'rb').read())

app = dash.Dash(__name__)#, external_stylesheets=external_stylesheets)
app.title="LORA DAQ Monitoring"
server = app.server

app.layout = html.Div([
    html.Img(src='data:image/png;base64,{}'.format(encoded_vub_logo.decode()),
            style={'width': '10%','vertical-align':'middle','display': 'inline-block'}),
    # html.Img(src='data:image/png;base64,{}'.format(encoded_ru_logo.decode()),
    #                    style={'width': '20%','vertical-align':'middle'}),
    html.H1('LORA DAQ Monitoring Dashboard',
            style={'textAlign':'center',
            'font-size':'300%',
            'display': 'inline-block'}),

    dcc.Dropdown(
        id='date_dropdown',
        searchable=False,
        options=get_file_list(),
        placeholder="Select a Date",
        value=glob_drop_down_value,
        clearable=False,
        style={'font-size': "110%",'width': '100%'}
    ),
    html.H3(id='reload_data',style={'color':'#1f77b4'}),

    html.H3(id='run_start_time',style={'color':'#1f77b4','display':'inline-block'}),
    html.H3(id='last_update_time',style={'display':'inline-block','color':'#ff7f0e'}),

    dcc.RadioItems(
        id='live_update_options',
        options=[
            {'label': 'Update Every 5s', 'value': 5*1e3},
            {'label': 'Update Every 15s', 'value': 15*1e3},
            {'label': 'Update Every 30s', 'value': 30*1e3},
            {'label': 'Static', 'value': 1e9},
        ],
        value=glob_radio_button_val,
        style={'font-size': "120%"}
    ),

    html.Hr(),
    html.H3(id='list_radio_trigs',
            # style={'color':'gray'}
            ),
    html.Hr(),

    dcc.Interval(
            id='interval-component',
            n_intervals=0
        ),

    dcc.Graph(
    id='ndet_vs_time',
    style={'width':'100%'}),

    html.Hr(),

    html.Span(id='hspace0',
              style={'display':'inline-block',
                      'width': 20}
            ),
    # dcc.Graph(
    #     id='evt_display',
    #     style={'width':500,
    #           'height':500,
    #           'display': 'inline-block'}
    #     ),
    html.Span(id='hspace03',
              style={'display':'inline-block',
                      'width': 50}
            ),
    dcc.Graph(
        id='core_hist2d',
        style={'width':500, 'height':500,'display': 'inline-block'}
        ),
    html.Span(id='hspace',
              style={'display':'inline-block',
                      'width': 50}
            ),
    dcc.Graph(
        id='charge_spectrum',
        style={'width':500,'display': 'inline-block'}
        ),

    html.Hr(),

    dcc.Graph(
        id='det_rate_in_last_ten_mins',
        style={'width':'100%'}
    ),

    html.Hr(),

    html.Label('Select Detector'),
    dcc.RadioItems(
        id='select_detector',
        options=[
            {'label': '1', 'value': 1},
            {'label': '2', 'value': 2},
            {'label': '3', 'value': 3},
            {'label': '4', 'value': 4},
            {'label': '5', 'value': 5},
            {'label': '6', 'value': 6},
            {'label': '7', 'value': 7},
            {'label': '8', 'value': 8},
            {'label': '9', 'value': 9},
            {'label': '10', 'value': 10},
            {'label': '11', 'value': 11},
            {'label': '12', 'value': 12},
            {'label': '13', 'value': 13},
            {'label': '14', 'value': 14},
            {'label': '15', 'value': 15},
            {'label': '16', 'value': 16},
            {'label': '17', 'value': 17},
            {'label': '18', 'value': 18},
            {'label': '19', 'value': 19},
            {'label': '20', 'value': 20},

        ],
        value=1
    ),

    dcc.Graph(id='time_plot_2'),

    html.Span(id='hspace05',
              style={'display':'inline-block',
                      'width': 250}
            ),
    dcc.Graph(
        id='charge_spectrum_det',
        style={'width':600,'display': 'inline-block'}
        ),

    html.Span(id='hspace06',
              style={'display':'inline-block',
                      'width': 50}
            ),
    dcc.Graph(
        id='peak_spectrum_det',
        style={'width':600,'display': 'inline-block'}
        ),

    ])

@app.callback(
    Output('date_dropdown', 'options'),
    [Input('interval-component', 'n_intervals')])
def update_filelist(n):
    return get_file_list()

@app.callback(
    Output('reload_data', 'children'),
    [Input('date_dropdown', 'value'),Input('interval-component', 'n_intervals')])
def update_output(value,n):
    global glob_drop_down_value
    latest_drop_down_value=sorted(glob.glob("logfiles/20*_array.log")
                                  +glob.glob("logfiles/*/20*_array.log"))[-1].split("_array")[0]

    if (value==glob_drop_down_value and value==latest_drop_down_value):
        #same as previous but latest file
        glob_drop_down_value=value
        load_station_file_current()
        load_array_file_current()
        error_msg="Page Updated as Time Changed."
    elif (value!=glob_drop_down_value):
        #not same as before.
        glob_drop_down_value=value
        load_station_file_current()
        load_array_file_current()
        error_msg="Page Updated as File Changed."
    else :
        error_msg="Page won't be updated with time when file chosen is not latest."
    return error_msg

@app.callback(Output('interval-component','interval'),
              [Input('live_update_options','value')])
def radio_button_liveupdate(value):
    global glob_radio_button_val
    glob_radio_button_val = value
    return glob_radio_button_val

@app.callback([
        Output('time_plot_2', 'figure'),
        Output('charge_spectrum_det', 'figure'),
        Output('peak_spectrum_det', 'figure'),
        ],
        [Input('select_detector', 'value')])
def radio_button_update(det):
    global glob_remember_det_selection, glob_sta_info_history, glob_sta_info_current
    #fig1=return_time_figure(det,glob_sta_info_history)
    fig2=return_time_figure(det,glob_sta_info_current)
    fig3=return_charge_spectrum_det(glob_arr_info_current,det)
    fig4=return_peak_spectrum_det(glob_arr_info_current,det)
    glob_remember_det_selection=det
    return fig2,fig3,fig4

@app.callback([Output('det_rate_in_last_ten_mins', 'figure'),
               Output('select_detector', 'value'),
               Output('last_update_time', 'children'),
               Output('ndet_vs_time', 'figure'),
               Output('core_hist2d', 'figure'),
               Output('charge_spectrum', 'figure'),
               Output('list_radio_trigs', 'children'),
               Output('run_start_time', 'children'),
               #Output('evt_display', 'figure'),
               ],
               [Input('reload_data', 'children')])
def live_update(n):
    if n=="Page won't be updated with time when file chosen is not latest.":
        PreventUpdate
    global glob_sta_info_current,glob_arr_info_current
    global glob_remember_det_selection

    fig1=return_det_id_figure(glob_sta_info_current, 'n_trigs', 'Blues', 'N Trigs per ~5 mins')
    value2=glob_remember_det_selection
    value3=get_last_load_time()
    fig4=return_time_figure_2(glob_arr_info_current)
    fig5=return_core_histogram(glob_arr_info_current)
    fig6=return_charge_spectrum(glob_arr_info_current)
    value7=return_radio_trig_text(glob_arr_info_current,'list')
    value8=return_run_start_time(glob_arr_info_current)
    #fig9=return_evt_display_histogram(glob_arr_info_current)

    return fig1, value2, value3, fig4, fig5, fig6, value7, value8

# debug=True enables hot reload. might be slowing things donw.
# app.run_server(debug=True,port=8010,threaded=True)
if __name__ == '__main__':
    #app.run_server(port=8010,threaded=True)
    app.run_server(port=8010)

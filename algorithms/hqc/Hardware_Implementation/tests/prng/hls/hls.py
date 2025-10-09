# -*- coding: utf-8 -*-
"""
Created on Mon Jun 29 10:08:38 2020

@author: a.dion

run vivado hls project and save file.
This function read data from an XML file and parse hls tcl command file
It runs the tcl, save the reports
inputs : 
    gen_proj.xml, template.xml

"""

# libraries
import shutil
import subprocess
from bs4 import BeautifulSoup as bs
from datetime import datetime
import os,glob


def parse_file(filepath, origin, replacer):
    """
    Parse text in place at given filepath

    Parameters
    ----------
    filepath : str
        Filepath for file_object to be parsed
    origin : str
        string to replace
    replacer : str
        string that replace
    Returns
    -------
    nothing
        
    """
    with open(filepath, 'rt') as template:
            content=template.read()
        
    content = content.replace(origin, replacer)
    
    with open(filepath, 'wt') as fout:    
        fout.write(content)



def findelt(root, name, result=None):
    if result is None:
        result = {}
    for elt in root:
        try :
            result[elt.tag] = elt.find(name).text
        except AttributeError:
            pass
        findelt(elt,name, result)
    return result
           
            
def findtiming(root, result=None):
    if result is None:
        result = {}
    for child in root.children :
        try :
            
            latency={}
            try :
                m={}
                if len(child.latency.contents)>1:
                    m['min'] = child.latency.min.string
                    m['max'] = child.latency.max.string
                    latency['cycles'] = m
                else :
                    m['min'] = child.latency.string
                    m['max'] = child.latency.string
                    latency['cycles'] = m
                result[child.name] = latency
            except (AttributeError, KeyError) :
                pass
            try :
                m={}
                if len(child.iterationlatency.contents)>1:
                    m['min'] = child.iterationlatency.min.string
                    m['max'] = child.iterationlatency.max.string
                    latency['iteration'] = m
                else :
                    m['min'] = child.iterationlatency.string
                    m['max'] = child.iterationlatency.string
                    latency['iteration'] = m
                result[child.name] = latency
            except (AttributeError, KeyError) :
                pass
            try :
                m={}
                m['min'] = child.pipelinedepth.string
                m['max'] = child.pipelinedepth.string
                latency['pipelinedepth'] = m
                result[child.name] = latency
            except (AttributeError, KeyError) :    
                pass
            try :
                m={}
                if len(child.tripcount.contents)>1:
                    m['min'] = child.tripcount.min.string
                    m['max'] = child.tripcount.max.string
                    latency['tripcount'] = m
                else :
                    m['min'] = child.tripcount.string
                    m['max'] = child.tripcount.string
                    latency['tripcount'] = m
                result[child.name] = latency
            except (AttributeError, KeyError) :
                pass
            findtiming(child, result)
        except (AttributeError, KeyError) :
            pass

    return result

def newtag(mini, maxi, htmlcomparereport, ntag):
    if mini == maxi:
        ltag = htmlcomparereport.new_tag("td", align="center", colspan="2", style="background-color:#CCECFF;")
        ltag.string = mini
        ntag.append(ltag)
    else:
        ltag = htmlcomparereport.new_tag("td", align="right", style="background-color:#CCECFF;")
        ltag.string = mini
        ntag.append(ltag)
        
        ltag = htmlcomparereport.new_tag("td", align="right", style="background-color:#CCECFF;")
        ltag.string = maxi
        ntag.append(ltag)
    return ntag

def parsetclcmdfile(solution, projectname, solutionname, functionname, description, clockperiod, templatepath, sourcepath, testbenchpath, flags):
    filename = f'{projectname}_{solutionname}.tcl'
    template=templatepath+'template.tcl'
    
    # copy template file to current directory
    try :
        dest = shutil.copy2(template, filename)
    except :
        print('Error while creating tcl file')
        return False
    
    if flags['csimflag'] == 'true' :
        csim = 'csim_design'
    else:
        csim =''
    parse_file(filename, '<CSIM>', csim)
    
    if flags['csynthflag'] == 'true' :
        csynth = 'csynth_design'
    else:
        csynth =''
    parse_file(filename, '<CSYNTH>', csynth)
    
    if flags['cosimflag'] == 'true' :
        cosim = 'cosim_design  -rtl vhdl -tool xsim'
    else:
        cosim =''
    parse_file(filename, '<COSIM>', cosim)
    
    if flags['implemflag'] == 'true' :
        implem = 'export_design -flow impl -rtl vhdl -format ip_catalog -description <DESCRIPTION> -vendor "ISAE" -display_name "<FUNCTION>"'
    else:
        implem =''
    parse_file(filename, '<IMPLEM>', implem)
    
    
    # parse template with project name
    parse_file(filename, '<PROJECT>', projectname)
    
    parse_file(filename, '<FUNCTION>', functionname)
    
    parse_file(filename, '<DESCRIPTION>', description)
    
    parse_file(filename, '<CLOCKPERIOD>', clockperiod)
    
    # parse template with solution name
    parse_file(filename, '<SOLUTION>', solutionname)
    
    try :
        attribut = solution.find('attribut').get('name')
        cflag = f'"-DHLS -D{attribut}"'
        cflagtb = f'"-I{sourcepath} -DHLS -D{attribut}"'
    except AttributeError :
        cflag = '"-DHLS"'
        cflagtb = f'"-I{sourcepath} -DHLS"'
        
    
    # Get project file names from TCL tree
    projectfiles= '\n'.join(f'add_files {sourcepath}{projectfile.get("name")} -cflags {cflag}' for projectfile in  projet.find_all('file'))
    # parse template with file names
    parse_file(filename, '<PROJECT_FILES>', projectfiles)
    
    # Get testbench file name from TCL tree
    tb_file = f'add_files -tb {testbenchpath}{projet.find("testbench").get("name")} -cflags {cflagtb} -csimflags {cflagtb}'
    # parse template with testbench names
    parse_file(filename, '<TB_FILE>', tb_file)


    # Get directive file if included
    try :
        directivefile = solution.find('directive').get('name')
        directive = f'source "{sourcepath}{directivefile}"'
    except AttributeError :
        directive = ''
  
    parse_file(filename, '<DIRECTIVE>', directive) 
    
    return filename
    
    
# XML file
xmlfile = 'gen_proj.xml'
# Path for various tools
vivado_path='C:/Xilinx/Vivado/2019.2/bin/'

# Alias for the bash script
vivado_hls=vivado_path+'vivado_hls'
vivado=vivado_path+'vivado'

# Date
date = datetime.now()

# import xml file
print(f'Read XML file : {xmlfile}')
with open(xmlfile, "r" ) as f:
    xml_cmd = f.read()
xmlcmd = bs(xml_cmd,features="lxml")

csimflag = xmlcmd.genprojet.csim.string
csynthflag = xmlcmd.genprojet.csynth.string
cosimflag = xmlcmd.genprojet.cosim.string
implemflag = xmlcmd.genprojet.implem.string
reportonlyflag = xmlcmd.genprojet.reportonly.string
flags = {'csimflag' : csimflag, 'csynthflag' : csynthflag,
         'cosimflag' : cosimflag, 'implemflag' : implemflag, 'reportonlyflag' : reportonlyflag}

for projet in xmlcmd.genprojet.find_all('projet'):

    # get project name from XML tree
    projectname = projet['name']

    # get function name from XML tree
    functionname = projet.find('function').get('name')

    # get description from XML tree
    description = f'"{projet.find("description").get("name")}"'
    
    # get clock period
    clockperiod = projet.find('clockperiod').get('value')
    
    # Path of the source folder
    sourcepath = projet.find('sourcepath').get('name')
    # Path of the testbench folder
    testbenchpath = projet.find('testbenchpath').get('name')
    # Path of the doc folder
    docpath = projet.find('docpath').get('name')
    
    # TCL template file
    templatepath = projet.find('templatepath').get('name')

    # Export report template
    template = f'{templatepath}compare_report.html'
    htmlreport = f'{docpath}{projectname}_compare_report.html'
    try :
        dest = shutil.copy2(template, htmlreport)
    except :
        print('Error while creating html file')
        break
    
    # htmlreport
    with open(htmlreport, "r" ) as f:
        html_doc = f.read()
            
    htmlcomparereport = bs(html_doc,features="lxml")
    htmlcomparereport.title.append(projectname)
    htmlcomparereport.h1.append(functionname)

    ##
    ## Summary
    ##

    product = 'Virtex-7'
    part = 'xc7vx485tffg1157-1'
    
    # insert today date
    tag=htmlcomparereport.find(id="date")
    nstr = f'{date.day}/{date.month}/{date.year}'
    tag.insert_after(nstr)
    
    # insert project name
    tag=htmlcomparereport.find(id="project")
    tag.insert_after(projectname)
    
    # insert product name 
    tag=htmlcomparereport.find(id="product")
    tag.insert_after(product)
    
    # insert target name
    tag=htmlcomparereport.find(id="target")
    tag.insert_after(part)
    
    html = htmlcomparereport.prettify()
    with open(htmlreport, "wt", encoding='utf-8') as file:
        file.write(str(html))    
    # Delete pre-existing project with the same name
    # process = subprocess.Popen([vivado_hls, 'open_project', '-reset', projectname], shell=True, 
    #                        stdout=subprocess.PIPE,
    #                        universal_newlines=True)  
       
    # Loop on solution for data types
    for solution in projet.find_all('solution'):
        
        solutionname = solution.get('name')
        print(f'\n---------------------> Create TCL file for project {projectname} ')
        print(f'\n---------------------> Solution {solutionname}\n\n')
    
        ##
        ## Parse vivado hls tcl comand file
        ##
        filename = parsetclcmdfile(solution, projectname, solutionname, functionname, description, clockperiod, templatepath, sourcepath, testbenchpath, flags)
        if filename == False :
            print('\n=====================================================================================')
            print(f'\n WARNING : break from tcl parsing for project {projectname}, solution {solutionname}')
            print('\n=====================================================================================')

            break
            
    
    
        print(f'\n---------------------> Run Vivado HLS for project {projectname}')
        print(f'\n---------------------> Solution {solutionname}\n\n')
        
        if reportonlyflag == 'false':
            ##
            ## Launch vivado HLS as a subprocess
            ##
            process = subprocess.Popen([vivado_hls, filename], shell=True, 
                                    stdout=subprocess.PIPE,
                                    universal_newlines=True)
        
            # Monitor the subprocess
            doclog = f'{docpath}vivado_hls_{projectname}_{solutionname}.log'
            with open(doclog, 'wt') as flog:    
                while True:
                    output = process.stdout.readline()
                    print(output.strip())
                    flog.write(f'{output.strip()}\n')
                    if output.strip().find('TESTS FAILED') > 0 :
                        print('** TESTS FAILED **')
                        # Process has finished, read rest of the output 
                        for output in process.stdout.readlines():
                            print(output.strip())
                            flog.write(f'{output.strip()}\n')
                        break
                    # Do something else
                    return_code = process.poll()
                    if return_code is not None:
                        print('RETURN CODE', return_code)
                        # Process has finished, read rest of the output 
                        for output in process.stdout.readlines():
                            print(output.strip())
                            flog.write(f'{output.strip()}\n')
                        break
        
        
            if implemflag == 'true' :
                print('\n---------------------> Fix bad property in generated files for GUI\n\n')
                apsfilepath = f'{projectname}/{solutionname}/{solutionname}.aps'
                with open(apsfilepath, "r" ) as f:
                    treeaps = f.read()
                
                rootaps = bs(treeaps,features="lxml")
                  
                try :
                    rootaps.exportdesign['flow'] = 'true'
                    rootaps.exportdesign['flowChoice'] = 'impl'
                    rootaps.exportdesign['rtlButton'] = 'true'
                    rootaps.exportdesign['rtlCombo'] = 'vhdl'
                except :
                    print('Error while modifying aps file')
          
                with open(apsfilepath, "wt", encoding='utf-8') as file:
                    file.write(str(rootaps))
        
        
        # Update html report
        print('\n---------------------> Export HTML report\n\n')
        # Synthesis report 
        reportcsynthpath = f'{projectname}/{solutionname}/syn/report/'
        reportcsynthfile = f'{reportcsynthpath}{functionname}_csynth.xml'
        # Implementation report
        reportpath = f'{projectname}/{solutionname}/impl/report/vhdl/'
        reportfile = f'{reportpath}{functionname}_export.xml'
        
        with open(htmlreport, "r" ) as f:
            html_doc = f.read()
            
        htmlcomparereport = bs(html_doc,features="lxml")
        
        # #
        # # Resource Usage estimated post-synthesis
        # #
        
        with open(reportcsynthfile, "r" ) as f:
              xml_report= f.read()
        xmlreportsynth = bs(xml_report,features="lxml")
        tagressources = xmlreportsynth.areaestimates.resources
        dsp = xmlreportsynth.dsp48e.string
        bram = xmlreportsynth.bram_18k.string
        lut = xmlreportsynth.lut.string
        ff = xmlreportsynth.ff.string
        targetclockperiod = xmlreportsynth.userassignments.targetclockperiod.string
        achievedclockperiod = xmlreportsynth.performanceestimates.summaryoftiminganalysis.estimatedclockperiod.string
       
        
        tag_instance=htmlcomparereport.find(id="resource_synth")
        
        # insert solution name
        ntag = htmlcomparereport.new_tag("tr")
        ltag = htmlcomparereport.new_tag("td", align="left", style="background-color:#CCECFF;")
        ltag.string = f'{solutionname}'
        ntag.append(ltag)
        
        # insert lut count
        ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
        ltag.string = lut
        ntag.append(ltag)
            
        # insert ff count
        ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
        ltag.string = ff
        ntag.append(ltag)
        
        # insert dsp count
        ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
        ltag.string = dsp
        ntag.append(ltag)
        
        # insert bram count
        ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
        ltag.string = bram
        ntag.append(ltag)
        
        tag_instance.append(ntag) 
        
        # #
        # # Resource Usage measured post-implementation
        # #
        
        tag_implem = htmlcomparereport.find(id='resource_implem')
        try :
            # Try if implementation report file exists
            with open(reportfile, "r" ) as f:
                xml_report= f.read()
            xmlreportimplem = bs(xml_report,features="lxml")
            tagressources = xmlreportimplem.areareport.resources
            slice = xmlreportimplem.slice.string
            lut = xmlreportimplem.lut.string
            ff = xmlreportimplem.ff.string
            dsp = xmlreportimplem.dsp.string
            bram = xmlreportimplem.bram.string
            targetclockperiod = xmlreportimplem.timingreport.targetclockperiod.string
            achievedclockperiod = xmlreportimplem.timingreport.achievedclockperiod.string
            
            
            # insert solution name
            ntag = htmlcomparereport.new_tag("tr")
            ltag = htmlcomparereport.new_tag("td", align="left", style="background-color:#CCECFF;")
            ltag.string = f'{solutionname}'
            ntag.append(ltag)
            
            # insert slice count
            ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
            ltag.string = slice
            ntag.append(ltag)
            
            # insert lut count
            ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
            ltag.string = lut
            ntag.append(ltag)
                
            # insert ff count
            ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
            ltag.string = ff
            ntag.append(ltag)
            
            # insert dsp count
            ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
            ltag.string = dsp
            ntag.append(ltag)
            
            # insert bram count
            ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
            ltag.string = bram
            ntag.append(ltag)
            
            tag_implem.append(ntag) 

        except FileNotFoundError :
            # Else extract info from synthesis report
            tag_implem.clear()
            tag_implem.find_previous('h2').clear()
            
        
        # #
        # # Timing Summary
        # #
        tag_sum = htmlcomparereport.find(id='timing_summary')
        with open(reportcsynthfile, "r" ) as f:
               xml_report= f.read()
        xmlreportlatency = bs(xml_report,features="lxml")
        
        clk_name = xmlreportlatency.interfacesummary.rtlports.find('name').string       
        
        # insert solution name
        ntag = htmlcomparereport.new_tag("tr")
        ltag = htmlcomparereport.new_tag("td", align="left", style="background-color:#CCECFF;")
        ltag.string = f'{solutionname}'
        ntag.append(ltag)
        
        # insert clock name
        ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
        ltag.string = clk_name
        ntag.append(ltag)
            
        # insert target clock
        ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
        ltag.string = f'{targetclockperiod} ns'
        ntag.append(ltag)
        
        # insert achieved clock
        ltag = htmlcomparereport.new_tag("td", align="center", style="background-color:#CCECFF;")
        ltag.string = f'{achievedclockperiod} ns'
        ntag.append(ltag)
            
        tag_sum.append(ntag)
            
            
        ##
        ## Latency report
        ##
        SummaryOfOverallLatency = xmlreportlatency.performanceestimates.summaryofoveralllatency
        
        
        
        tag_lat = htmlcomparereport.find(id='latency_summary')
        ntag = htmlcomparereport.new_tag("tr")
        ltag = htmlcomparereport.new_tag("td", align="left", style="background-color:#CCECFF;")
        ltag.string = f'{solutionname}'
        ntag.append(ltag)
        
        mini = SummaryOfOverallLatency.find('best-caselatency').string
        maxi = SummaryOfOverallLatency.find('worst-caselatency').string
        ntag = newtag(mini, maxi, htmlcomparereport, ntag)
            
        mini = SummaryOfOverallLatency.find('best-caserealtimelatency').string
        maxi = SummaryOfOverallLatency.find('worst-caserealtimelatency').string
        ntag = newtag(mini, maxi, htmlcomparereport, ntag)
        
            
        tag_lat.append(ntag)
        
   
        
        
        tag_instance = htmlcomparereport.find(id='timing_detail')
        del tag_instance['id']
        
        # ntagh3 = htmlcomparereport.new_tag("h3")
        # ntagh3['id'] ='timing_detail'
        # ntagh3['style'] = "background-color:#f7f7ff;"
        # ntagh3.string = f'Implemented version : {solutionname}'
        # tag_instance.append(ntagh3)
        
        ntagli1 = htmlcomparereport.new_tag("li")
        ntagbr = htmlcomparereport.new_tag("br")
        
        ntagli1['id'] ='timing_detail'
        ltag = htmlcomparereport.new_tag("b")
        ltag.string = f'Solution name : {solutionname}'
        ltag['style'] = "background-color:#f7f7ff;"
        
        ntagli1.append(ltag)
        ntagli1.insert(1, ntagbr)
      
        ntagul = htmlcomparereport.new_tag("ul")
        
        
        
        ##
        ## Instance report
        ##
        iterfile = glob.glob(os.path.join(reportcsynthpath, '*_csynth.xml'))
        ntagli2 = htmlcomparereport.new_tag("li")
        ntagli2.string = 'Instances'
        if sum(1 for filename in iterfile) > 1:
            
            
            
            
            ntagtab = htmlcomparereport.new_tag("table")
            ntagtab['border'] = 1
            ntagtab['cellspacing'] = "0"
            
            ntagtr = htmlcomparereport.new_tag("tr")
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['align'] = "center"
            ntagth['rowspan'] = "2"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth.string = 'Instance Name'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['colspan'] = "2"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'Latency (cycles)'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['colspan'] = "2"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'Latency (absolute)'
            ntagtr.append(ntagth)
            
            ntagtab.append(ntagtr)
            
            ntagtr = htmlcomparereport.new_tag("tr")
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['align'] = "center"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth.string = 'min'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'max'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'min'
            ntagtr.append(ntagth)
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'max'
            ntagtr.append(ntagth)
            
            ntagtab.append(ntagtr)
            
            							        
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            for filename in iterfile:
                
                with open(filename, "r" ) as f:
                    xml_report= f.read()
                xmlreportimpleminstance = bs(xml_report,features="lxml")
                
                instancename = xmlreportimpleminstance.userassignments.topmodelname.string
                if instancename != projectname :
                
                    
                    SummaryOfOverallLatency = xmlreportimpleminstance.performanceestimates.summaryofoveralllatency
                    
                    ntag = htmlcomparereport.new_tag("tr")
                    ltag = htmlcomparereport.new_tag("td", align="left", style="background-color:#CCECFF;")
                    ltag.string = instancename
                    ntag.append(ltag)
                    mini = SummaryOfOverallLatency.find('best-caselatency').string
                    maxi = SummaryOfOverallLatency.find('worst-caselatency').string
                    ntag = newtag(mini, maxi, htmlcomparereport, ntag)
                        
                    mini = SummaryOfOverallLatency.find('best-caserealtimelatency').string
                    maxi = SummaryOfOverallLatency.find('worst-caserealtimelatency').string
                    ntag = newtag(mini, maxi, htmlcomparereport, ntag)
                    
                    ntagtab.append(ntag)
                    
            
        else :
            ntagtab = htmlcomparereport.new_tag("br")
            #ntagtab.string = 'Not applicable'
        
        
        ntagli2.append(ntagtab)
            
        ntagul.append(ntagli2)
        
        
        
        
        
        
        ##
        ## Loop report
        ##
        try :
            looplatency = findtiming(xmlreportlatency.summaryoflooplatency)
            ntagli2 = htmlcomparereport.new_tag("li")
            ntagli2.string = 'Loops'
            
            ntagtab = htmlcomparereport.new_tag("table")
            ntagtab['border'] = 1
            ntagtab['cellspacing'] = "0"
            
            ntagtr = htmlcomparereport.new_tag("tr")
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['align'] = "center"
            ntagth['rowspan'] = "2"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth.string = 'Loop name'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['colspan'] = "2"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'Latency (cycles)'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['colspan'] = "2"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'Iteration Latency'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['colspan'] = "2"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'Trip count'
            ntagtr.append(ntagth)
            
            ntagtab.append(ntagtr)
            
            ntagtr = htmlcomparereport.new_tag("tr")
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['align'] = "center"
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth.string = 'min'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'max'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'min'
            ntagtr.append(ntagth)
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'max'
            ntagtr.append(ntagth)
            
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'min'
            ntagtr.append(ntagth)
            ntagth = htmlcomparereport.new_tag("th")
            ntagth['style'] = "background-color:#CCECFF;"
            ntagth['align'] = "center"
            ntagth.string = 'max'
            ntagtr.append(ntagth)
            
            ntagtab.append(ntagtr)
            
            
            
            
            
            
            
            
            for loopname in looplatency:
                ntag = htmlcomparereport.new_tag("tr")
                
                ltag = htmlcomparereport.new_tag("td", align="left", style="background-color:#CCECFF;")
                ltag.string = loopname
                ntag.append(ltag)
                
                mini = looplatency[loopname]['cycles']['min']
                maxi = looplatency[loopname]['cycles']['max']
                ntag = newtag(mini, maxi, htmlcomparereport, ntag)
                try :
                    mini = looplatency[loopname]['iteration']['min']
                    maxi = looplatency[loopname]['iteration']['max']
                    ntag = newtag(mini, maxi, htmlcomparereport, ntag)
                except KeyError:
                    mini = looplatency[loopname]['pipelinedepth']['min']
                    maxi = looplatency[loopname]['pipelinedepth']['max']
                    ntag = newtag(mini, maxi, htmlcomparereport, ntag)
                
                mini = looplatency[loopname]['tripcount']['min']
                maxi = looplatency[loopname]['tripcount']['max']
                ntag = newtag(mini, maxi, htmlcomparereport, ntag)
    
                
                ntagtab.append(ntag)
                
                
        except AttributeError: 
            ntagtab = htmlcomparereport.new_tag("br")
            #ntagtab.string = 'Not applicable'
       
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        ntagli2.append(ntagtab)
        
        ntagul.append(ntagli2)
  								      
        
        
        
        
        
        ntagli1.append(ntagul)
        tag_instance.insert_after(ntagli1)

        # tag_instance.append(ntagul)
        
        
        
        
        
        
        
        
        
        ##
        ## Copy all extracted info in the html doc file
        ##
        html = htmlcomparereport.prettify(formatter="html5")
        with open(htmlreport, "wt", encoding='utf-8') as file:
            file.write(str(html))
    

    
    
    
    
    
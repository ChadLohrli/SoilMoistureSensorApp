//
//  ViewController.swift
//  SoilMoistureSensorApp
//
//  Created by Chad Lohrli on 2/7/17.
//  Copyright © 2017 Chad Lohrli. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    
    //raw data variables
    @IBOutlet weak var adc: UILabel!
    @IBOutlet weak var resistance: UILabel!
    @IBOutlet weak var voltage: UILabel!
    @IBOutlet weak var temperature: UILabel!
    
    let sparkfunURL:String = "http://192.168.4.1/read" //chip's URL

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func refresh(_ sender: Any) {
        
        parseData()
        
    }
    
    func parseData(){
        
        //TODO: implement a check to see if we are connected to sensor
        
        
        let url = URL(string: sparkfunURL) //url instantiation
        let data = try? Data(contentsOf: url!) //data instantiation
        var jsonResult: [String:AnyObject] = [:] //declare json variable 
        
        do {
            
            if(data != nil) {
                
                //parse json data
                jsonResult = try JSONSerialization.jsonObject(with: data!, options: JSONSerialization.ReadingOptions.allowFragments) as! [String:AnyObject]
                
                
                //if values are not nil, set respective values
                if let adcValue = jsonResult["adcValue"] {
                    adc.text = adcValue.stringValue
                }
                
                if let resistanceValue = jsonResult["resistance"] {
                    resistance.text = resistanceValue.stringValue
                }
                if let voltageValue = jsonResult["voltage"] {
                    voltage.text = voltageValue.stringValue
                }
                
                if let temperatureValue = jsonResult["temperature"] {
                    temperature.text = temperatureValue.stringValue
                }
            
                
            }else{
                adc.text = "No Data Recieved"
            }
            
        }catch let error as NSError {
            print(error)
        }
        
    }

}


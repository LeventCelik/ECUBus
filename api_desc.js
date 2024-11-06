/* eslint-disable no-unused-vars */

/**
 * @description Control UI progress bar display, user need close display manual.
 * @param  {boolean} showBar - Enable progress bar show in US.
 * @param  {number} currentProgressPercent - Progress percent
 * @param  {string} progressBarName='main' - Progress bar name
 * @example 
 * this.progress(true,30,'flash driver')
 * this.progress(false,100,'flash driver)
 */
function progress(showBar, currentProgressPercent, progressBarName = 'main') {
  
}
/**
 * @description Store value in key-value in a temporary database.
 * @param {string} key  
 * @param {any} value
 * @param {boolean} keep - determines if value should be kept between each start. If false, the value will be clear on start. Else, the value will be kept always.
 */
function set(key, value, keep = false) {

}
/**
 * @description Get value from key-value database
 * @param {string} key - key to fetch value from
 * @returns {any}
 */
function get(key){

}

/**
 * @description Print verbose level information to log window
 * @param {any} message 
 */
function verbose(message) {
   
}

/**
 * @description Print error level information to log window
 * @param {any} message 
 */
 function error(message) {
   
}

/**
 * @description Print info level information to log window
 * @param {any} message 
 */
 function info(message) {
   
}

/**
 * @description Print debug level information to log window
 * @param {any} message 
 */
 function debug(message) {
   
}
/**
 * @description Flexbile log print.
 * @param {any} message 
 * @param {string} type - 'verbose','error','info','debug' 
 */
function log(message, type = 'debug') {

}
/**
 * @description Open a file and store fd in internal.
 * @param {string} filename - File name
 * @param {string} flag - File operation flag
 * @example
 * this.openFile('fw.bin','w')
 * this.writeFile('hello world')
 * this.closeFile()
 */
function openFile(filename, flag = 'r'){

}
/**
 * @description Read data from last open file, return length maybe less than expected size.
 * @param {number} expectedSize - expected read size from file
 * @returns {array}
 */
function readFile(expectedSize) {
    
}
/**
 * @description Write data into last open file.
 * @param {buffer} data 
 */
function writeFile(data) {

}
/**
 * @description Change schedule table next frame payload content.
 * @param {string} name - Next frame payload name
 * @param {array} value 
 * @example 
 * this.changeNextFrame("memorySize",[0,0,0,0x55])
 */
function changeNextFrame(name, value = []){

}
/**
 * 
 * @param {number} serviceID - UDS service id
 * @param {array} messagePayload - UDS payload value except serivce id 
 * @param {func} handleFunction - UDS response handle function
 * @param {func} prehandleFunction - UDS pre-handle function
 * @example
 * this.inserItem(0x36,[0x55,0x33],(writeData,readData)=>{
 *      this.log(writeData)
 * })
 */
function insertItem(serviceID, messagePayload, handleFunction = (writeData, readData) => { return true }, prehandleFunction = (writeData) => { }){

}

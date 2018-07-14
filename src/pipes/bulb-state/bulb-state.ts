import { Pipe, PipeTransform } from '@angular/core';

/**
 * Generated class for the BulbStatePipe pipe.
 *
 * See https://angular.io/api/core/Pipe for more info on Angular Pipes.
 */
@Pipe({
  name: 'bulbState',
})
export class BulbStatePipe implements PipeTransform {
  /**
   * Takes a value and makes it lowercase.
   */
  transform(value: string, ...args) {
    if (value == '0'){
      return "ON";
    }
    return "OFF";
    // return value.toLowerCase();
  }
}

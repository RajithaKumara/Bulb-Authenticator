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

  transform(value: string, ...args) {
    if (value == '0') {
      return "OFF";
    }
    return "ON";
  }
}

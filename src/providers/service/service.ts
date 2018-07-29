import { HttpClient, HttpErrorResponse, HttpHeaders } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { _throw } from 'rxjs/observable/throw'
import { catchError, retry } from 'rxjs/operators';

/*
  Generated class for the ServiceProvider provider.

  See https://angular.io/guide/dependency-injection for more info on providers
  and Angular DI.
*/
@Injectable()
export class ServiceProvider {
  httpOption: any;

  constructor(public http: HttpClient) {
    this.httpOption = {
      headers: new HttpHeaders({
        'Content-Type': 'application/json'
      })
    }
  }

  get(url) {
    return this.http.get(url/* , this.httpOption */)
    // .pipe(
    //   retry(3), // retry a failed request up to 3 times
    //   catchError(this.handleError) // then handle the error
    // )
  }

  private handleError(error: HttpErrorResponse) {
    let err;
    if (error.error instanceof ErrorEvent) {
      // A client-side or network error occurred. Handle it accordingly.
      err = 'An error occurred:' + error.error.message;
    } else {
      // The backend returned an unsuccessful response code.
      // The response body may contain clues as to what went wrong,
      err = `Backend returned code ${error.status}, ` + `body was: ${error.error}`;
      // return error.error;
    }
    // return an observable with a user-facing error message
    // return _throw('Something bad happened; please try again later.');
    return _throw(err);
  }

}

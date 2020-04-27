#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <pqxx/pqxx>

using namespace std;

int main(int argc, char* argv[]){
        //connecting to the database
        string connection_string("host=cs-db1.csil.sfu.ca port=5432 dbname=cmpt354-student user=student password=studentpassword");
        pqxx::connection connect(connection_string);
        pqxx::nontransaction query(connect);

        //querying the database
        bool continue_program = true;
        while (continue_program == true){

                //determining user's query
                string user_query;
                cout << "Please provide the name of the query you would like to make (accepted format: Q1,Q2,Q3 etc.): ";
                cin >> user_query;
                cin.ignore();
                cout << endl;

                //if user chooses Q1
                if (user_query == "Q1"){
                        //determine user's chosen month
                        string user_month;
                        cout << "Please provide a month (accepted format: january, february etc.): ";
                        cin >> user_month;
                        cout << endl;

                        //begin query
                        pqxx::result q1_result = query.exec(
                            "select q.id, q.title "
                            "from (select c.id, c.title, p.requested,c.deadline, c.status as call_status, p.status as proposal_status, count(*) as participants "
                            "from call c, proposal p, collaborator c1 "
                            "where c.id = p.id and p.id = c1.proposalid "
                            "group by c.id, p.requested, p.status) q "
                            "where (q.requested > 20000.00 or q.participants > 10) and q.proposal_status = 'submitted' and q.call_status = 'open' and extract(month from q.deadline) = " + month_convert(user_month));

                        //return results
                        cout << "ID, Title of competitions that match query criteria:" << endl;
                        cout << endl;
                        if (q1_result.size() == 0){
                                cout << "No entries in database match query criteria" << endl;
                                cout << endl;
                        }else{
                                for (int rownum = 0; rownum < q1_result.size(); rownum++){
                                        const pqxx::result::tuple row = q1_result[rownum];
                                        for (int colnum = 0; colnum < row.size(); colnum++){
                                                const pqxx::result::field field = row[colnum];
                                                cout << field.c_str() << '\t';
                                        }
                                        cout << endl;
                                }
                        }

                //if user chooses Q2    
                }else if (user_query == "Q2"){
                        //define variables
                        string user_areas;
                        string user_principle_investigators;
                        vector<string> areas;
                        vector<string> principle_investigators;

                        //obtain areas and pi's from user
                        cout << "Please provide a comma-separated list (no spaces) of preferred areas (lower case): ";
                        cin >> user_areas;
                        cout << "Please provide a comma-separated list (no spaces) of the IDs of your preferred principle investigators: ";
                        cin >> user_principle_investigators;
                        cout << endl;

                        //parse user-given lists
                        stringstream area_stream(user_areas);
                        while (area_stream.good()){
                                string substr;
                                getline(area_stream, substr, ',');
                                areas.push_back(substr);
                        }

                        stringstream pi_stream(user_principle_investigators);
                        while (pi_stream.good()){
                                string substr;
                                getline(pi_stream, substr, ',');
                                principle_investigators.push_back(substr);
                        }

                        //set up areas and pi string for query
                        string areas_final = " q.area = '" + areas[0] + "' ";
                        for (int i = 1; i<areas.size(); i++){
                                areas_final += "or q.area = '" + areas[i] + "' ";
                        }

                        string pi_final = "or q.id = '" + principle_investigators[0] + "' ";
                        for (int i = 1; i<principle_investigators.size(); i++){
                                pi_final += "or q.id = '" + principle_investigators[i] + "' ";
                        }

                        string final_query = areas_final + pi_final;

                        //begin query
                        pqxx::result q2_result = query.exec(
                                "select q.id, q.title "
                                "from (select c.id, c.title, p.pi as principle_investigator, r.firstname, c.area, c.status as call_status, p.requested, p.status as proposal_status, count(*) as participants "
                                "from call c, proposal p, collaborator c1, researcher r "
                                "where c.id = p.id and p.id = c1.proposalid and r.id = p.pi "
                                "group by c.id, p.requested, p.status, p.pi, r.firstname) q "
                                "where (q.requested > 20000.00 or q.participants > 10) and q.proposal_status = 'submitted' and q.call_status = 'open' and (" + final_query + ") ");

                        //return results
                        cout << "ID, Title of competitions that match query criteria:" << endl;
                        cout << endl;
                        if (q2_result.size() == 0){
                                cout << "No entries in database match query criteria" << endl;
                                cout << endl;
                        }else{
                                for (int rownum = 0; rownum < q2_result.size(); rownum++){
                                        const pqxx::result::tuple row = q2_result[rownum];
                                        for (int colnum = 0; colnum < row.size(); colnum++){
                                                const pqxx::result::field field = row[colnum];
                                                cout << field.c_str() << '\t';
                                        }
                                        cout << endl;
                                }
                        }
                }else if (user_query == "Q3"){
                        //define variables
                        string user_area;
                        cout << "Please provide a single area (lower case): ";
                        cin >> user_area;

                        //begin query
                        pqxx::result q3_result = query.exec(
                                "select q.title "
                                "from (select * "
                                "from proposal p, call c "
                                "where p.id = c.id) q "
                                "where q.requested = (select max (p1.requested) "
                                "from proposal p1, call c1 "
                                "where p1.id = c1.id and c1.area = '" + user_area + "')");

                        //return result
                        cout << "Title of proposal that matches query criteria:" << endl;
                        cout << endl;
                        if (q3_result.size() == 0){
                                cout << "No entries in database match query criteria" << endl;
                                cout << endl;
                        }else{
                                for (int rownum = 0; rownum < q3_result.size(); rownum++){
                                const pqxx::result::tuple row = q3_result[rownum];
                                        for (int colnum = 0; colnum < row.size(); colnum++){
                                                const pqxx::result::field field = row[colnum];
                                                cout << field.c_str() << '\t';
                                        }
                                        cout << endl;
                                }
                        }

                }else if (user_query == "Q4"){
                        //define variables
                        string user_date;
                        cout << "Please provide a date (accepted format: yyyy-mm-dd): ";
                        cin >> user_date;

                        //begin query
                        pqxx::result q4_result = query.exec(
                                "select q.title "
                                "from (select * "
                                "from call c, proposal p "
                                "where c.id = p.id and p.status = 'awarded') q "
                                "where q.submitted < '" + user_date +  "' and q.requested = (select max (p1.requested) "
                                "from proposal p1, call c1 "
                                "where p1.id = c1.id and p1.status = 'awarded')");

                        //return result
                        cout << "Title of proposal that matches query criteria:" << endl;
                        cout << endl;
                        if (q4_result.size() == 0){
                                cout << "No entries in database match query criteria" << endl;
                                cout << endl;
                        }else{
                                for (int rownum = 0; rownum < q4_result.size(); rownum++){
                                        const pqxx::result::tuple row = q4_result[rownum];
                                        for (int colnum = 0; colnum < row.size(); colnum++){
                                                const pqxx::result::field field = row[colnum];
                                                cout << field.c_str() << '\t';
                                        }
                                        cout << endl;
                                }
                        }

                }else if (user_query == "Q5"){
                        //define variables
                        string user_area;
                        cout << "Please provide an area (lower case): ";
                        getline(cin, user_area);

                        //begin query
                        pqxx::result q5_result = query.exec(
                                "select round(avg(requested_awarded_discrepancy),2) as discrepancy "
                                "from (select abs(q.requested - q.awarded) as requested_awarded_discrepancy "
                                "from (select * "
                                "from call c, proposal p "
                                "where c.id = p.id and c.area = '" + user_area + "') q) h ");

                        //return results
                        cout << "Average requested/awarded discrepancy that matches query criteria:" << endl;
                        cout << "(If result is empty, it means that area currently does not have any proposal that has been awarded yet)" << endl;
                        cout << endl;
                        if (q5_result.size() == 0){
                                cout << "No entries in database match query criteria" << endl;
                                cout << endl;
                        }else{
                                for (int rownum = 0; rownum < q5_result.size(); rownum++){
                                        const pqxx::result::tuple row = q5_result[rownum];
                                        for (int colnum = 0; colnum < row.size(); colnum++){
                                                const pqxx::result::field field = row[colnum];
                                                cout << field.c_str() << '\t';
                                        }
                                        cout << endl;
                                }
                        }

                }else if (user_query == "Q6"){

                        //determine user's proposal
                        string user_proposal_id;
                        cout << "Please provide a proposal ID: ";
                        cin >> user_proposal_id;
                        cout << endl;

                        //check if user wants a list of reviewers
                        string user_response;
                        cout << "Would you like a list of reviewers who are not in conflict with the proposal being reviewed? (y/n): ";
                        cin >> user_response;
                        cout << endl;

                        if (user_response == "y"){
                                pqxx::result q6_reviewer_list = query.exec(
                                        "select * "
                                        "from researcher r "
                                        "where r.id not in (select pc1.reviewer_in_conflict "
                                        "from proposalconflict pc1 "
                                        "where pc1.proposalid = " + user_proposal_id + ") and r.id not in (select q.reviewerid "
                                        "from (select r.reviewerid, count(*) as number_of_proposals "
                                        "from proposal p, review r "
                                        "where p.id = r.proposalid "
                                        "group by r.reviewerid) q "
                                        "where q.number_of_proposals > 3) ");

                                if (q6_reviewer_list.size() == 0){
                                        cout << "All reviewers are in conflict with the proposal or have been assigned to more than 3 proposals" << endl;
                                        cout << "The program has terminated, you may wish to re-enter Q6 with a different proposal" << endl;
                                        exit(1);

                                }else{
                                        for (int rownum = 0; rownum < q6_reviewer_list.size(); rownum++){
                                                const pqxx::result::tuple row = q6_reviewer_list[rownum];
                                                for (int colnum = 0; colnum < row.size(); colnum++){
                                                        const pqxx::result::field field = row[colnum];
                                                        cout << field.c_str() << '\t';
                                                }
                                                cout << endl;
                                        }
                                }
                        }
                        cout << endl;
                        cin.ignore();

                        //Determine user's list of researchers they would like to assign
                        string user_researchers;
                        cout << "Please provide a comma-separated list (no spaces) of the IDs of reviewers you would like to assign to this proposal: ";
                        getline(cin, user_researchers);
                        cout << endl;

                        //parse user's list into a vector string
                        vector<string> user_reviewers;
                        stringstream s_stream(user_researchers);
                        while (s_stream.good()){
                                string substr;
                                getline(s_stream, substr, ',');
                                user_reviewers.push_back(substr);
                        }

                        //iterate through reviewers and update them in database
                        for (int i = 0; i<user_reviewers.size(); i++){
                                string researcher = user_reviewers[i];

                                //find the researcher's id
                                pqxx::result user_reviewerid = query.exec(
                                        "select r.id as researcher_id "
                                        "from researcher r "
                                        "where r.id = '"+ researcher +"' ");
                                string user_reviewer_id = user_reviewerid[0][0].c_str();

                                //find the proposal's deadline
                                pqxx::result user_proposaldeadline = query.exec(
                                        "select c.deadline "
                                        "from call c, proposal p "
                                        "where c.id = p.id and p.id = "+ user_proposal_id +" ");
                                string user_proposal_deadline = user_proposaldeadline[0][0].c_str();

                                //set up connection
                                pqxx::connection connectQ6(connection_string);
                                pqxx::nontransaction queryQ6(connectQ6);

                                //assign reviewer to proposal
                                queryQ6.exec(
                                        "insert into review(id, reviewerid, proposalid, deadline, submitted) values "
                                        "(default, "+ user_reviewer_id +", "+ user_proposal_id +", '"+ user_proposal_deadline +"', default) ");
                                queryQ6.commit();
                        }

                }else{
                        //Determine user's room # and date
                        string user_date;
                        string user_room_number;
                        cout << "Please enter a 4-digit room number: ";
                        cin >> user_room_number;
                        cout << endl;

                        cout << "Please enter a date (accepted format: yyyy-mm-dd): ";
                        cin >> user_date;
                        cout << endl;

                        //Check if room is available at user-selected date
                        pqxx::result check_room = query.exec(
                                "select * "
                                "from meeting "
                                "where room_number = " + user_room_number + " and date = '" + user_date + "'");

                        //If the room is already booked for that day
                        if (check_room.size() > 0){
                                cout << "There is already a meeting booked in this room" << endl;
                                cout << "The program has terminated, you may wish to enter Q7 with a different room number and date" << endl;
                                cout << endl;
                                exit(0);
                        }

                        //Room is available for that day
                        pqxx::result get_calls = query.exec(
                                "select * "
                                "from call ");

                        cout << "Here are the current competitions (calls)" << endl;
                        for (int rownum = 0; rownum < get_calls.size(); rownum++){
                                const pqxx::result::tuple row = get_calls[rownum];
                                for (int colnum = 0; colnum < row.size(); colnum++){
                                        const pqxx::result::field field = row[colnum];
                                        cout << field.c_str() << '\t';
                                }
                                cout << endl;
                        }
                        cout << endl;
                        cin.ignore();

                        //Determine user's calls
                        string user_calls;
                        cout << "Please provide a comma-separated list (no spaces) of 3 competitions (IDs) that you wish to be discussed: ";
                        getline(cin, user_calls);
                        cout << endl;

                        //parse user's calls into a vector string
                        vector<string> user_calls_array;
                        stringstream s_stream(user_calls);
                        while (s_stream.good()){
                                string substr;
                                getline(s_stream, substr, ',');
                                user_calls_array.push_back(substr);
                        }

                        //iterate through calls 
                        for (int i = 0; i<user_calls_array.size(); i++){
                                string call = user_calls_array[i];
                                string eligible_reviewers = "{";

                                //Find all eligible reviewers for the given call
                                pqxx::result find_reviewers = query.exec(
                                        "select r.id "
                                        "from researcher r "
                                        "where r.id not in (select c.researcherid as researcher_and_pi "
                                        "from collaborator c, researcher r, call ca, proposal p "
                                        "where c.proposalid = p.id and p.callid = ca.id and ca.id = " + user_calls_array[i] + ") ");

                                //create string *array* with reviewers from the query
                                for (int rownum = 0; rownum < find_reviewers.size()-1; rownum++){
                                        const pqxx::result::tuple row = find_reviewers[rownum];
                                        for (int colnum = 0; colnum < row.size(); colnum++){
                                                const pqxx::result::field field = row[colnum];
                                                eligible_reviewers = eligible_reviewers + field.c_str() + ",";
                                        }
                                }

                                eligible_reviewers = eligible_reviewers + find_reviewers[find_reviewers.size()-1][find_reviewers[find_reviewers.size()-1].size()-1].c_str() + "}";

                                //Check if reviewers are available to discuss the call
                                pqxx::result check_reviewers = query.exec(
                                        "select * "
                                        "from meeting "
                                        "where date = '" + user_date + "' and '" + eligible_reviewers + "' && participants ");

                                if (check_reviewers.size() > 0){
                                        cout << "One or more reviewers are not available on this day for call " << call << endl;
                                        cout << endl;
                                }else{
                                        cout << "All reviewers are able to discuss competition " << call <<  " on the given day!" << endl;
                                        cout << endl;

                                        //set up connection
                                        pqxx::connection connectQ7(connection_string);
                                        pqxx::nontransaction queryQ7(connectQ7);

                                        //insert into database
                                        queryQ7.exec("insert into meeting values (default, " + call + ", '" + eligible_reviewers + "', '" + user_date + "', " + user_room_number + ") ");
                                        queryQ7.commit(); 
                                }
                        }
                }
                
                //check if user wants to continue making queries in program
                string user_program_continue;
                cout << "Would you like to make another query? (y/n): ";
                cin >> user_program_continue;

                if (user_program_continue == "y"){
                        continue_program = true;
                }else{
                        continue_program = false;
                }
        }
}
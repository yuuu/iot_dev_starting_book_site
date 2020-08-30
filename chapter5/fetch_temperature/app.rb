require 'json'
require 'aws-sdk'

def lambda_handler(event:, context:)
  dynamodb = Aws::DynamoDB::Client.new

  begin
    result = dynamodb.query(
      table_name: 'Temperature',
      key_condition_expression: '#id = :id and #timestamp >= :timestamp',
      expression_attribute_names: { '#id': 'id', '#timestamp': 'timestamp' },
      expression_attribute_values: {
        ':id': event['pathParameters']['id'],
        ':timestamp': (Time.now.to_i - 100) * 1000
      }
    )
  rescue  Aws::DynamoDB::Errors::ServiceError => error
    return response 500, { message: error.message }
  end

  body = result.items.map{|r| { timestamp: r['timestamp'].to_i,  value: r['value'].to_f }}
  response 200, body
end

def response(status, result)
  {
    statusCode: status,
    headers: {
      'Access-Control-Allow-Headers': 'Content-Type',
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET'
    },
    body: body.to_json
  }
end